
#ifndef Slice_H
#define Slice_H

#include "label.H"

#include "faceList.H" //required for appendFaces

#include <functional>
#include <algorithm>

#include "sliceMap.H"
#include "sliceMeshHelper.H"

namespace Foam
{

// Forward declaration
class Offsets;
class Slice;

template<typename Container>
label numCellsFromSlice( const Container&, const Slice& );

template<typename Container, typename Operator>
void appendTransformed( Container& ret,
                        Container& input,
                        Operator operation );

class Slice {

    label partition_{};

    label lowerCellId_{};
    label upperCellId_{};

    label lowerPointId_{};
    label upperPointId_{};

    label bottom_{};
    label top_{};

    std::shared_ptr<sliceMap> cellMap_{};
    std::shared_ptr<sliceMap> pointMap_{};
    
    std::shared_ptr<sliceMap> mapping_{};

    template< typename Container >
    void append( const Container& );

public:

    // Constructors
    Slice() = default;
    Slice( const label&, const Foam::Offsets&, const Foam::Offsets&  );

    auto partition() { return partition_; }

    Slice cells() {
        bottom_ = lowerCellId_; 
        top_ = upperCellId_; 
        mapping_ = cellMap_;
        return *this;
    }

    Slice points() {
        bottom_ = lowerPointId_;
        top_ = upperPointId_; 
        mapping_ = pointMap_;
        return *this;
    }

    bool operator()( const label& id ) { return ( bottom_ <= id && id < top_ ); }

    auto shift( const label& id ) { return id - bottom_; }

    template< typename Container >
    auto shiftRange( Container& list ) 
    { 
        std::transform( std::begin( list ), std::end( list ),
                        std::begin( list ),
                        [ this ]( const auto& id ) 
                        { return shift( id ); } );
        return list; 
    }

    label convert( const label& id ) { 
        return this->operator()( id ) ? shift( id ) : mapping_->operator[]( id ); 
    }

    template< typename Container >
    void appendCells( const Container& );

    template< typename Container >
    void appendPoints( const Container& );

    std::set<label> missingPoints( const faceList& faces ) {
        points();
        auto pointsInFaces = Foam::pointSubset( faces );
        std::set<label> missingPointIDs{};
        std::copy_if( std::begin( pointsInFaces ), std::end( pointsInFaces ),
                      std::inserter( missingPointIDs, missingPointIDs.end() ),
                      [ this ] ( const label& id ) { 
                          return !( mapping_->exist( id ) ) 
                                 && 
                                 !( this->operator()( id ) ); } );
        return missingPointIDs;
    }
    

};

}

template< typename Container >
void Foam::Slice::append( const Container& list ) {
    mapping_->append( list );
}

template< typename Container >
void Foam::Slice::appendCells( const Container& list ) {
    cells();
    append( list );
}

template< typename Container >
void Foam::Slice::appendPoints( const Container& list ) {
    points();
    append( list );
}

template<typename Container>
Foam::label Foam::numCellsFromSlice( const Container& neighbours, 
                                     const Foam::Slice& slice )
{
    return std::count_if( std::begin( neighbours ), 
                          std::end( neighbours ), 
                          slice );
}

template<typename Container, typename Operator>
void Foam::appendTransformed( Container& ret,
                              Container& input,
                              Operator operation )
{
    std::transform( std::begin( input ),
                    std::end( input ),
                    std::begin( input ),
                    operation );
    ret.append( input );
}

#endif
