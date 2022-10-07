
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

//template<typename Container, typename UnaryPredicate>
//std::set<label> pointSet( const Container&, UnaryPredicate );

//template<typename UnaryPredicate>
//std::set<label> pointSet( const faceList&, UnaryPredicate );

//template<typename Container>
//std::set<label> pointSet( const Container& );

template<typename SomeList, typename IndexList>
SomeList extractor( const SomeList& input, const IndexList& extractorList );

// Forward declaration
class Offsets;

class sliceProcPatch {

    static label instanceCount_;

    label id_{};
    label numNonProcPatches_{};

    Slice slice_{};

    labelList localFaceIDs_{};
    labelList localPointIDs_{};

    word procBoundaryName_{};

public:

    // Constructors
    sliceProcPatch( const Foam::Slice&,
                    const std::vector<label>&,
                    const Foam::label& );

    // Copy constructor
    sliceProcPatch( const sliceProcPatch& );

    // Copy assignment operator
    sliceProcPatch& operator=( const sliceProcPatch& );

    // Destructor
    ~sliceProcPatch(){ --instanceCount_; }

    void swap( sliceProcPatch& ) noexcept;

    label id() { return id_; }

    word name() { return procBoundaryName_; }

    label partner() { return slice_.partition(); }

    template<typename Container>
    label countFaces( const Container& neighbours ) {
        slice_.cells();
        return std::count_if( neighbours.begin(), neighbours.end(), slice_ );
    }

    void determineFaceIDs( const std::vector<label>& );

    void determinePointIDs( const faceList&, const label& );

    void appendOwner( Foam::labelList&, Foam::labelList& );

    void encodePatch( std::vector<label>& );
    void encodePatch( std::vector<label>&, const label& );

    template<typename FaceList>
    FaceList extractFaces( const FaceList& input ) {
        return extractor( input, localFaceIDs_ );
    }

    pointField extractPoints( const pointField& input );
};

void swap( sliceProcPatch&, sliceProcPatch& ) noexcept;

}

//template<typename Container, typename UnaryPredicate>
//std::set<Foam::label> Foam::pointSet( const Container& list, UnaryPredicate pred ) {
    //std::set<Foam::label> pointIDs{};
    //std::copy_if( list.begin(), list.end(),
                  //std::inserter( pointIDs, pointIDs.end() ),
                  //pred );
    //return pointIDs;
//}

//template<typename UnaryPredicate>
//std::set<Foam::label> Foam::pointSet( const Foam::faceList& faces, UnaryPredicate pred ) {
    //std::set<Foam::label> pointIDs{};
    //for ( const auto& face : faces ) {
        //auto tmp = Foam::pointSet( face, pred );
        //pointIDs.insert( std::begin( tmp ), std::end( tmp ) );
        ////std::copy_if( face.begin(), face.end(),
                      ////std::inserter( pointIDs, pointIDs.end() ),
                      ////pred );
    //}
    //return pointIDs;
//}
//
//template<typename Container>
//std::set<Foam::label> Foam::pointSet( const Container& data ) {
    //return Foam::pointSet( data, [](auto){ return true; } );
//}


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
