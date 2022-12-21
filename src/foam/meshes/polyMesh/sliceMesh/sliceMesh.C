/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | foam-extend: Open Source CFD
   \\    /   O peration     | Version:     4.1
    \\  /    A nd           | Web:         http://www.foam-extend.org
     \\/     M anipulation  | For copyright notice see file Copyright
-------------------------------------------------------------------------------
License
    This file is part of foam-extend.

    foam-extend is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by the
    Free Software Foundation, either version 3 of the License, or (at your
    option) any later version.

    foam-extend is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with foam-extend.  If not, see <http://www.gnu.org/licenses/>.

\*---------------------------------------------------------------------------*/

#include "sliceMesh.H"
#include "sliceMeshHelper.H"
//#include "slicePermutation.H"
#include "nonblockConsensus.H"

#include "adiosFileStream.H"
#include "adiosWritePrimitives.H"
#include "adiosReadPrimitives.H"

#include "processorPolyPatch.H"

#include <numeric>
#include <cmath>


// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

defineTypeNameAndDebug(Foam::sliceMesh, 0);


// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //

void Foam::sliceMesh::readMesh()
{
    // Reading owners
    std::vector<Foam::label> ownerStarts;
    if ( Pstream::parRun() ) {
        Foam::label start = Pstream::myProcNo();
        const Foam::label count = 1;
        std::vector<Foam::label> partitionStarts;
        adiosReadToContainer( "mesh", "partitionStarts", partitionStarts, start, count + 1 );
        cellOffsets_.set( partitionStarts.back() );
        Foam::label cellStart = cellOffsets_.lowerBound( Pstream::myProcNo() );
        Foam::label cellCount = cellOffsets_.count( Pstream::myProcNo() );

        adiosReadToContainer( "mesh", "ownerStarts", ownerStarts, cellStart, cellCount + 1 );
    } else {
        adiosReadToContainer( "mesh", "ownerStarts", ownerStarts );
        cellOffsets_.set( ownerStarts.size() );
    }

    // Reading neighbours and patches
    Foam::label faceStart = ownerStarts.front();
    Foam::label faceCount = ownerStarts.back() - ownerStarts.front();
    newGlobalNeighbours_.resize( faceCount, 0 );
    adiosReadPrimitives( "mesh", "neighbours", newGlobalNeighbours_.data(), faceStart, faceCount );
    adiosReadToContainer( "mesh", "neighbours", globalNeighbours_, faceStart, faceCount );

    // Offsets for surfaceFields ( owning faces )
    // Offsets of all faces
    Foam::Offsets faceOffsets_;
    faceOffsets_.set( globalNeighbours_.size() );
    // Offsets of all internal faces
    auto internal = []( label index ) { return index >= 0; };
    Foam::label numInternalFaces = std::count_if( std::begin( globalNeighbours_ ),
                                                  std::end( globalNeighbours_ ),
                                                  internal );
    internalSurfaceFieldOffsets_.set( numInternalFaces, true );
    // Offsets of all physical boundary faces
    for ( Foam::label patchi = 0; patchi < numBoundaries_; ++patchi ) {
        auto slicePatchId = Foam::encodeSlicePatchId( patchi );
        auto boundary = [ slicePatchId ]( Foam::label index ) { return index == slicePatchId; };
        Foam::Offsets patchOffsets;
        Foam::label numBoundaryPatchFaces = std::count_if( std::begin( globalNeighbours_ ),
                                                           std::end( globalNeighbours_ ),
                                                           boundary );
        patchOffsets.set( numBoundaryPatchFaces, true );
        boundarySurfacePatchOffsets_.push_back( patchOffsets );
    }

    // Reading faceStarts and faces
    std::vector<Foam::label> faceStarts;
    adiosReadToContainer( "mesh", "faceStarts", faceStarts, faceStart, faceCount + 1 );

    std::vector<Foam::label> linearizedFaces;
    Foam::label facePointStart = faceStarts.front();
    Foam::label facePointCount = faceStarts.back() - faceStarts.front();
    adiosReadToContainer( "mesh", "faces", linearizedFaces, facePointStart, facePointCount );

    // Create and communicate offset lists for mesh entity exchange
    pointOffsets_.set( *std::max_element( linearizedFaces.begin(), linearizedFaces.end() ) + 1 );

    // Reading points
    Foam::labelList pointStart{2};
    pointStart[0] = pointOffsets_.lowerBound( Pstream::myProcNo() );
    pointStart[1] = 0;
    Foam::labelList pointCount{2};
    pointCount[0] = pointOffsets_.count( Pstream::myProcNo() );
    pointCount[1] = 3;
    allPoints_.resize( pointOffsets_.count( Pstream::myProcNo() ) );
    if ( pointCount[0] > 0 ) {
        adiosReadPrimitives( "mesh", "points", allPoints_.data(), pointStart, pointCount );
    }

    localOwner_ = serializeOwner( ownerStarts );
    globalFaces_ = deserializeFaces( faceStarts, linearizedFaces );

    cellSlice_ = Foam::Slice( Pstream::myProcNo(), cellOffsets_ );
    pointSlice_ = Foam::Slice( Pstream::myProcNo(), pointOffsets_ );

    if ( Pstream::parRun() ) {
       commSlicePatches();
       commSharedPoints();
       renumberFaces();
    }

    sliceablePermutation_ = slicePermutation( globalNeighbours_ );
}


void Foam::sendSliceFaces( std::pair<Foam::label, Foam::label> sendPair,
                           Foam::Offsets& cellOffsets,
                           Foam::Offsets& pointOffsets,
                           std::vector<Foam::label>& globalNeighbours,
                           Foam::faceList& globalFaces,
                           Foam::DynamicList<Foam::label>& newGlobalNeighbours,
                           Foam::pointField& allPoints_,
                           std::vector<Foam::sliceProcPatch>& sliceProcPatches,
                           Foam::label numBoundaries )
{
    Foam::label myProcNo = Pstream::myProcNo();
    Foam::label partition = sendPair.first;
    Foam::OPstream toPartition( Pstream::blocking, partition, 0, 0 );

    Foam::Slice slice( partition, cellOffsets );
    Foam::sliceProcPatch procPatch( slice, globalNeighbours, numBoundaries );

    // Face Stuff
    auto sendFaces = procPatch.extractFaces( globalFaces );
    toPartition << sendFaces;

    // Owner Stuff
    auto sendNeighbours = procPatch.extractFaces( newGlobalNeighbours );
    toPartition << sendNeighbours;

    // Point Stuff
    procPatch.determinePointIDs( sendFaces, pointOffsets.lowerBound( myProcNo ) );
    auto sendPoints = procPatch.extractPoints( allPoints_ );
    toPartition << sendPoints;

    // procBoundary Stuff: Track face swapping indices for processor boundaries;
    procPatch.encodePatch( globalNeighbours );
    sliceProcPatches.push_back( procPatch );
}

void Foam::recvSliceFaces( std::pair<Foam::label, Foam::label> recvPair,
                           Foam::faceList& globalFaces,
                           Foam::labelList& localOwner,
                           Foam::Offsets& cellOffsets,
                           Foam::Offsets& pointOffsets,
                           Foam::Slice& cellSlice,
                           Foam::Slice& pointSlice,
                           Foam::pointField& allPoints_,
                           std::vector<Foam::sliceProcPatch>& sliceProcPatches,
                           std::vector<Foam::label>& globalNeighbours,
                           Foam::label numBoundaries )
{
    label partition = recvPair.first;
    label numberOfPartitionFaces = recvPair.second;
    IPstream fromPartition( Pstream::blocking, partition, 0, 0 );

    // Face Communication
    faceList recvFaces( numberOfPartitionFaces );
    fromPartition >> recvFaces;
    Foam::appendTransformed( globalFaces, recvFaces,
                             [] ( auto& face )
                             { return face.reverseFace(); } );

    // Owner Communication
    labelList recvOwner( numberOfPartitionFaces );
    fromPartition >> recvOwner;
    Foam::appendTransformed( localOwner, recvOwner,
                             [ &cellSlice ]( const auto& id )
                             { return cellSlice.convert( id ); } );

    // Identify points from slice/partition that associate with the received faces
    Foam::Slice recvPointSlice( partition, pointOffsets );
    auto pointIDs = Foam::pointSubset( recvFaces, recvPointSlice );
    // Append new point IDs to point mapping
    // TODO: Create proper state behaviour in Slice
    pointSlice.append( pointIDs );

    // Point Communication
    pointField recvPoints( pointIDs.size() );
    fromPartition >> recvPoints;
    allPoints_.append( recvPoints );

    // ProcBoundary Stuff
    Foam::Slice recvCellSlice( partition, cellOffsets );
    sliceProcPatch procPatch( recvCellSlice, globalNeighbours, numBoundaries );
    procPatch.encodePatch( globalNeighbours, numberOfPartitionFaces );
    sliceProcPatches.push_back( procPatch );
}

// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //

Foam::faceList Foam::sliceMesh::deserializeFaces( const std::vector<Foam::label>& faceStarts,
                                                  const std::vector<Foam::label>& linearizedFaces ) {
    Foam::faceList globalFaces( faceStarts.size() - 1 );
    for ( size_t i = 0; i < globalFaces.size(); ++i ) {
        globalFaces[i].resize( faceStarts[i+1] - faceStarts[i] );
        std::copy( linearizedFaces.begin() + (faceStarts[i] - faceStarts[0]),
                   linearizedFaces.begin() + (faceStarts[i+1] - faceStarts[0]),
                   globalFaces[i].begin() );
    }
    return globalFaces;
}

Foam::labelList Foam::sliceMesh::serializeOwner( const std::vector<Foam::label>& ownerStarts ) {
    Foam::label ownerCellID = 0;
    Foam::labelList localOwner;
    localOwner.setSize( ownerStarts.back() - ownerStarts.front() );
    for( size_t i = 0; i < ownerStarts.size()-1; ++i) {
       std::fill( localOwner.begin() + ownerStarts[i] - ownerStarts.front(),
                  localOwner.begin() + ownerStarts[i+1] - ownerStarts.front(),
                  ownerCellID );
       ++ownerCellID;
    }
    return localOwner;
}

void Foam::sliceMesh::commSlicePatches() {

    auto sendNumPartitionFaces = Foam::numFacesToExchange( cellOffsets_, pointOffsets_, globalNeighbours_ );
    auto recvNumPartitionFaces = Foam::nonblockConsensus( sendNumPartitionFaces );

    for ( const auto& sendPair : sendNumPartitionFaces )
    {
        sendSliceFaces( sendPair,
                        cellOffsets_,
                        pointOffsets_,
                        globalNeighbours_,
                        globalFaces_,
                        newGlobalNeighbours_,
                        allPoints_,
                        slicePatches_,
                        numBoundaries_ );
    }

    for ( const auto& recvPair : recvNumPartitionFaces )
    {
        recvSliceFaces( recvPair,
                        globalFaces_,
                        localOwner_,
                        cellOffsets_,
                        pointOffsets_,
                        cellSlice_,
                        pointSlice_,
                        allPoints_,
                        slicePatches_,
                        globalNeighbours_,
                        numBoundaries_ );
    }
}

void Foam::sliceMesh::commSharedPoints()
{
    auto myProcNo = Pstream::myProcNo();
    //TODO: Refactor
    std::map<label, std::vector<label> > sendPointIDs{};
    auto missingPointIDs = Foam::missingPoints( globalFaces_, pointSlice_ ); //slice_.missingPoints( globalFaces_ );
    for ( label partition = 0; partition < myProcNo; ++partition ) {
        Foam::Slice recvPointSlice( partition, pointOffsets_ );
        auto globallySharedPoints = Foam::pointSubset( missingPointIDs, recvPointSlice );
        if ( !globallySharedPoints.empty() ) {
            std::vector<label> tmp( globallySharedPoints.begin(), globallySharedPoints.end() );
            sendPointIDs[ partition ] = std::move( tmp );
        }
    }
    auto recvPointIDs = Foam::nonblockConsensus( sendPointIDs, MPI_LONG );

    for ( const auto& commPair : recvPointIDs ) {
        auto partition = commPair.first;
        auto sharedPoints = commPair.second;
        pointSlice_.convert( sharedPoints );
        pointField pointBuf = Foam::extractor( allPoints_, sharedPoints );
        OPstream::write( Pstream::blocking,
                         partition,
                         reinterpret_cast<const char*>( pointBuf.data() ),
                         pointBuf.size() * 3 * sizeof(scalar) );
    }

    for ( const auto& commPair : sendPointIDs ) {
        auto partition = commPair.first;
        auto sharedPoints = commPair.second;
        label count = sharedPoints.size();
        pointField pointBuf;
        pointBuf.resize( count );
        IPstream::read( Pstream::blocking,
                        partition,
                        reinterpret_cast<char*>( pointBuf.data() ),
                        count * 3 * sizeof(scalar) );
        for ( const auto& point : pointBuf ) allPoints_.append( point );
        pointSlice_.append( sharedPoints );
    }
}

void Foam::sliceMesh::renumberFaces()
{
    for ( auto& face : globalFaces_ ) {
        std::transform( face.begin(), face.end(), face.begin(),
                        [ this ] ( const auto& id )
                        { return pointSlice_.convert( id ); } );
    }
}

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::sliceMesh::sliceMesh(const Foam::polyMesh& pm)
:
    MeshObject<polyMesh, sliceMesh>(pm),
    numBoundaries_(pm.boundaryMesh().size()),
    boundaryGlobalIndex_(pm.boundaryMesh().size())
{
    readMesh();
}

// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

Foam::labelList Foam::sliceMesh::polyNeighbours() {
    // TODO: copy poly neighbours into labelList
    std::vector<Foam::label> neighbours{};
    sliceablePermutation_.copyPolyNeighbours( neighbours );
    cellSlice_.convert( neighbours );
    labelList polyNeighbours( neighbours.size() );
    std::copy( std::begin( neighbours ),
               std::end( neighbours ),
               std::begin( polyNeighbours ) );
    return polyNeighbours;
}

Foam::labelList Foam::sliceMesh::polyOwner() {
    auto owner = localOwner_;
    sliceablePermutation_.mapToPoly( owner );
    return owner;
}

Foam::faceList Foam::sliceMesh::polyFaces() {
    auto faces = globalFaces_;
    sliceablePermutation_.mapToPoly( faces );
    return faces;
}

Foam::pointField Foam::sliceMesh::polyPoints() {
    return allPoints_;
}

std::vector<Foam::label> Foam::sliceMesh::polyPatches() {
    std::vector<label> patches{};
    sliceablePermutation_.copyPolyPatches( patches );
    return patches;
}

std::vector<Foam::sliceProcPatch> Foam::sliceMesh::procPatches() {
    return slicePatches_;
}

void Foam::sliceMesh::polyPatches( polyBoundaryMesh& boundary )
{
    if ( Pstream::parRun() )
    {
        label myProcNo = Pstream::myProcNo();
        label numInternalFaces = std::count_if( std::begin( globalNeighbours_ ),
                                                std::end( globalNeighbours_ ),
                                                [] ( const auto& id )
                                                { return id >= 0; } );
        // Set correct boundary patches and add processor boundary patches
        std::vector<label> patches = polyPatches();

        const Foam::polyPatchList& meshPatches = boundary;
        std::set<label> patchSet( patches.begin(), patches.end() );
        Foam::List<Foam::polyPatch*> procPatches( patchSet.size(), reinterpret_cast<Foam::polyPatch*>(0) );
        Foam::label nthPatch = 0;
        Foam::label nLocalPatches = 0;
        for ( label patchi = 0; patchi < meshPatches.size(); ++patchi ) {
           auto slicePatchId = Foam::encodeSlicePatchId( patchi );
           if ( patchSet.count( slicePatchId ) ) {
               Foam::label patchSize = std::count( patches.begin(), patches.end(), slicePatchId );
               auto patchStartIt = std::find( patches.begin(), patches.end(), slicePatchId );
               Foam::label patchStart = numInternalFaces + std::distance( patches.begin(), patchStartIt );
               procPatches[ nLocalPatches ] = meshPatches[ patchi ].clone( boundary,
                                                                           nthPatch,
                                                                           patchSize,
                                                                           patchStart ).ptr();
               ++nLocalPatches;
           }
           ++nthPatch;
        }

        auto sliceProcPatches = this->procPatches();
        for ( label patchi = 0; patchi < sliceProcPatches.size(); ++patchi ) {
           auto sliceProcPatchId = sliceProcPatches[ patchi ].id();
           Foam::label patchSize = std::count( patches.begin(), patches.end(), sliceProcPatchId );
           auto patchStartIt = std::find( patches.begin(), patches.end(), sliceProcPatchId );
           Foam::label patchStart = numInternalFaces + std::distance( patches.begin(), patchStartIt );
           procPatches[ nLocalPatches ] = new Foam::processorPolyPatch( sliceProcPatches[ patchi ].name(),
                                                                  patchSize,
                                                                  patchStart,
                                                                  nthPatch,
                                                                  boundary,
                                                                  myProcNo,
                                                                  sliceProcPatches[ patchi ].partner() );
           ++nLocalPatches;
           ++nthPatch;
        }

        boundary.clear();

        boundary.setSize(procPatches.size());

        // Copy the patch pointers
        forAll (procPatches, pI)
        {
            boundary.set(pI, procPatches[pI]);
        }
    }
}


const Foam::globalIndex&
Foam::sliceMesh::boundaryGlobalIndex(label patchId)
{
    if (!boundaryGlobalIndex_.set(patchId))
    {
        boundaryGlobalIndex_.set
        (
            patchId,
            new globalIndex(mesh().boundaryMesh()[patchId].size())
        );
    }

    return boundaryGlobalIndex_[patchId];
}


// ************************************************************************* //
