
#ifndef sliceProcPatch_H
#define sliceProcPatch_H

#include "labelList.H"
#include "faceList.H"

#include "Slice.H"

#include <set>
#include <vector>

#include <algorithm>

namespace Foam
{

template<typename SomeList, typename IndexList>
SomeList extractor( const SomeList& input, const IndexList& extractorList );

template<typename Container>
label countNeighOfSlice( const Container& neighbours, const Slice& slice ) {
    return std::count_if( neighbours.begin(), neighbours.end(), slice );
}

// Forward declaration
class Offsets;

class sliceProcPatch {

    using index_container = labelList;
    using iterator = index_container::iterator;

    static label instanceCount_;

    label id_{};
    label numNonProcPatches_{};

    Slice slice_{};

    // Indices in face ordered list
    // that belong to this processor boundary
    index_container localFaceIDs_{};

    // Indices in point ordered list
    // that belong to this processor boundary
    index_container localPointIDs_{};

    word procBoundaryName_{};

public:

    // Constructors
    sliceProcPatch( const Foam::Slice&,
                    const Foam::labelList&,
                    const Foam::label& );

    // Copy constructor
    sliceProcPatch( const sliceProcPatch& );

    // Copy assignment operator
    sliceProcPatch& operator=( const sliceProcPatch& );

    // Destructor
    ~sliceProcPatch(){ --instanceCount_; }

    void swap( sliceProcPatch& ) noexcept;

    // Returns the processor boundary index
    // with account of physical boundaries
    label id() { return id_; }

    word name() { return procBoundaryName_; }

    // Returns the neighbouring processor
    label partner() { return slice_.partition(); }

    void determineFaceIDs( const labelList& );

    void determinePointIDs( const faceList&, const label& );

    void appendOwner( Foam::labelList&, Foam::labelList& );

    void encodePatch( labelList& );
    void encodePatch( std::vector<label>&, const label& );
    void encodePatch( labelList&, const label& );

    template<typename FaceList>
    FaceList extractFaces( const FaceList& input ) {
        return extractor( input, localFaceIDs_ );
    }

    pointField extractPoints( const pointField& input );

    iterator begin() { return localFaceIDs_.begin(); }
    iterator end() { return localFaceIDs_.end(); }
    label size() { return localFaceIDs_.size(); }
};

void swap( sliceProcPatch&, sliceProcPatch& ) noexcept;

}

template<typename SomeList, typename IndexList>
SomeList Foam::extractor( const SomeList& input, const IndexList& extractorList ) {
    SomeList output;
    output.resize( extractorList.size() );
    std::transform( std::begin( extractorList ),
                    std::end( extractorList ),
                    std::begin( output ),
                    [ &input ] ( const auto& id )
                    { return input[ id ]; } );
    return output;
}


#endif
