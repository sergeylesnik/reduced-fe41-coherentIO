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

#include <numeric>
#include <cmath>


// * * * * * * * * * * * * * Standalone Conversions in Namespace   * * * * * * * * //

Foam::label Foam::encodeSlicePatchId( const Foam::label& Id ) {
    return -(Id+1);
}


Foam::label Foam::decodeSlicePatchId( const Foam::label& encodedId ) {
    return -(encodedId+1);
}


Foam::label Foam::sliceNeighbourId( const Foam::label& polyFaceId, const Foam::polyMesh& mesh ) {
    auto patchId = mesh.boundaryMesh().whichPatch( polyFaceId );
    return (patchId != -1) ? encodeSlicePatchId( patchId ) : mesh.faceNeighbour()[ polyFaceId ];
}


// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //


void Foam::sliceMesh::createPointPermutation( Foam::faceList& faces, const Foam::label& nPoints ) {
    permutationToPolyPoint_.resize( nPoints, -1 );
    permutationToSlicePoint_.resize( nPoints, -1 );
    Foam::label slicePointId = 0;
    for ( const auto& face : faces ) {
        for ( const auto& polyPointId : face ) {
            if ( permutationToPolyPoint_[ polyPointId ] == -1 )
            {
                permutationToPolyPoint_[ polyPointId ] = slicePointId;
                permutationToSlicePoint_[ slicePointId ] = polyPointId;
                ++slicePointId;
            }
        }
    }
}


std::vector<std::pair<Foam::label, Foam::label> >
Foam::sliceMesh::createPolyNeighbourPermutation( const std::vector<Foam::label>& sliceNeighbours ) {
    auto polySlicePairs{ generateIndexedPairs( sliceNeighbours ) };
    partitionByFirst( polySlicePairs );
    return polySlicePairs;
}


std::vector<Foam::label>
Foam::sliceMesh::createSlicePermutation( const Foam::labelList& polyOwner ) {
    std::vector<Foam::label> indices{};
    indexIota( indices, polyOwner.size(), 0 );
    indexSort( indices, polyOwner );
    return indices;
}


void Foam::sliceMesh::renumberToSlice( Foam::faceList& input ) {
    renumberFaces( input, permutationToPolyPoint_ );
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::sliceMesh::sliceMesh( const std::vector<Foam::label>& sliceNeighbours ) :
    polyNeighboursPermutation_{ createPolyNeighbourPermutation( sliceNeighbours ) },
    polyNeighboursAndPatches_{ extractNth( polyNeighboursPermutation_,
                                 [](const auto& pair)
                                 { return pair.first; } ) },
    permutationToPoly_{ extractNth( polyNeighboursPermutation_,
                                    [](const auto& pair)
                                    { return pair.second; } ) }
{
    polyNeighboursPermutation_.clear();
}

Foam::sliceMesh::sliceMesh( const polyMesh& mesh ) :
    permutationToSlice_{ createSlicePermutation( mesh.faceOwner() ) }
{
    Foam::faceList sliceFaces{ mesh.allFaces() };
    mapToSlice( sliceFaces );
    createPointPermutation( sliceFaces, mesh.nPoints() );
}

Foam::sliceMesh::sliceMesh( const Foam::labelList& faceOwner,
                            const Foam::faceList& allFaces,
                            const Foam::label& nPoints ) :
    permutationToSlice_{ createSlicePermutation( faceOwner ) }
{
    Foam::faceList sliceFaces{ allFaces };
    mapToSlice( sliceFaces );
    createPointPermutation( sliceFaces, nPoints );
}

// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

// Transformations to sliceMesh

void Foam::sliceMesh::mapToSlice( Foam::pointField& allPoints ) {
    applyPermutation( allPoints, permutationToSlicePoint_ );
}


Foam::label Foam::sliceMesh::mapToSlice( const Foam::label& id ) {
    return permutationToSlice_[ id ];
}


Foam::faceList Foam::sliceMesh::generateSlice( Foam::faceList& allFaces ) {
    mapToSlice( allFaces );
    renumberToSlice( allFaces );
    return allFaces;
}


Foam::labelList Foam::sliceMesh::generateSlice( Foam::labelList& sliceNeighbours, const Foam::polyMesh& mesh ) {
    sliceNeighbours.resize( permutationToSlice_.size() );
    std::transform( permutationToSlice_.begin(),
                    permutationToSlice_.end(),
                    sliceNeighbours.begin(),
                    [ &mesh ]( const auto& polyFaceId )
                    { return sliceNeighbourId( polyFaceId, mesh ); } );
    return sliceNeighbours;
}


// Transformations to polyMesh

Foam::label Foam::sliceMesh::mapToPoly( const Foam::label& id ) {
    return permutationToPoly_[ id ];
}


void Foam::sliceMesh::resetPolyPatches( polyBoundaryMesh& boundary )
{
    auto patchBegin = polyNeighboursAndPatches_.begin();
    auto patchEnd = polyNeighboursAndPatches_.begin();
    for( Foam::label patchId = 0; patchId < boundary.size(); ++patchId ) {
        resetNextPatch( patchBegin,
                        patchEnd,
                        boundary[ patchId ],
                        patchId );
    }
}


// ************************************************************************* //
