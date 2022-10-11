
#include "sliceProcPatch.H"

#include "Offsets.H"
#include "slicePermutation.H"

#include <set>

Foam::label Foam::sliceProcPatch::instanceCount_ = 0;

Foam::sliceProcPatch::sliceProcPatch( const Foam::Slice& slice,
                                      const std::vector<label>& neighbours,
                                      const Foam::label& numBoundaries )
    : numNonProcPatches_{ numBoundaries }
    , slice_{ slice }
    , localFaceIDs_{ countFaces( neighbours ) }
    , procBoundaryName_{ word("procBoundary") +
                         Foam::name( Pstream::myProcNo() ) +
                         word("to") +
                         Foam::name( slice_.partition() ) } 
{
    determineFaceIDs( neighbours );
    ++instanceCount_;
    id_ = Foam::encodeSlicePatchId( (numNonProcPatches_ - 1) + instanceCount_ );
}

// Copy constructor
Foam::sliceProcPatch::sliceProcPatch( const sliceProcPatch& other ) :
    id_{ other.id_ },
    slice_{ other.slice_ },
    localFaceIDs_{ other.localFaceIDs_ },
    localPointIDs_{ other.localPointIDs_ },
    procBoundaryName_{ other.procBoundaryName_ }
{ 
    ++instanceCount_; 
}

Foam::sliceProcPatch&
Foam::sliceProcPatch::operator=( Foam::sliceProcPatch const& other ) 
{
    Foam::sliceProcPatch tmp( other );
    swap( tmp );
    return *this;
}

void Foam::sliceProcPatch::swap( Foam::sliceProcPatch& other ) noexcept 
{
    using std::swap;
    swap( id_, other.id_ );
    swap( instanceCount_, other.instanceCount_ );
    swap( slice_, other.slice_ );
    swap( localFaceIDs_, other.localFaceIDs_ );
    swap( localPointIDs_, other.localPointIDs_ );
    swap( procBoundaryName_, other.procBoundaryName_ );
}

void Foam::sliceProcPatch::determineFaceIDs( const std::vector<label>& neighbours )
{
    auto faceIdIter = localFaceIDs_.begin();
    for ( auto neighboursIter = std::begin( neighbours );
          neighboursIter != std::end( neighbours );
          ++neighboursIter ) {
        if ( slice_( *neighboursIter ) ) {
            *faceIdIter = std::distance( neighbours.begin(), neighboursIter );
            ++faceIdIter;
        }
    }
}

void Foam::sliceProcPatch::determinePointIDs( const Foam::faceList& faces,
                                              const Foam::label& bottomPointId ) {
    auto pred = [ bottomPointId ] ( const auto& id )
                { return bottomPointId <= id; };
    auto pointIDs = Foam::pointSubset( faces, pred );
    localPointIDs_.resize( pointIDs.size() );
    std::transform( pointIDs.begin(),
                    pointIDs.end(),
                    localPointIDs_.begin(),
                    [ &bottomPointId ] ( const auto& id )
                    { return id - bottomPointId; } );
}

void Foam::sliceProcPatch::appendOwner( Foam::labelList& owner, 
                                        Foam::labelList& recvOwner )
{
    std::transform( recvOwner.begin(),
                    recvOwner.end(),
                    recvOwner.begin(),
                    [ this ] ( const auto& id )
                    { return slice_.shift( id ); } );
    owner.append( recvOwner );
}

void Foam::sliceProcPatch::encodePatch( std::vector<Foam::label>& neighbours ) {
    std::transform( std::begin( neighbours ), std::end( neighbours ),
                    std::begin( neighbours ),
                    [ this ] ( const auto& cellId ) 
                    { return slice_( cellId ) ? id_ : cellId; } );
}


void Foam::sliceProcPatch::encodePatch( std::vector<Foam::label>& neighbours,
                                        const Foam::label& numPatchFaces ) {
    std::fill_n( std::back_inserter( neighbours ), numPatchFaces, id_ );
}

Foam::pointField Foam::sliceProcPatch::extractPoints( const Foam::pointField& input ) {
    Foam::pointField output;
    output.resize( localPointIDs_.size() );
    std::transform( std::begin( localPointIDs_ ),
                    std::end( localPointIDs_ ),
                    std::begin( output ),
                    [ &input ] ( const auto& id )
                    { return input[ id ]; } );
    return output;
}

void Foam::swap( Foam::sliceProcPatch& a, Foam::sliceProcPatch& b ) noexcept {
   a.swap( b );
}

