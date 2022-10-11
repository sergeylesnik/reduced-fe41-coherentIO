
#ifndef Slice_H
#define Slice_H

#include "label.H"

#include "faceList.H" //required for appendFaces

#include <functional>
#include <algorithm>
#include <utility> // std::declval

#include "sliceMap.H"
#include "sliceMeshHelper.H"

namespace Foam
{

template<typename T, typename _ = void>
struct is_range : std::false_type {};

template<typename... Ts>
struct is_range_helper {};

template<typename T>
struct is_range<
        T,
        std::conditional_t<
            false,
            is_range_helper<
                decltype(std::declval<T>().begin()),
                decltype(std::declval<T>().end())
                >,
            void
            >
        > : public std::true_type {};

// Forward declaration
class Offsets;
class Slice;

std::set<label> missingPoints( const faceList& faces,
                               const Slice& slice );

template<typename Container>
label numCellsFromSlice( const Container&, const Slice& );

template<typename Container, typename Operator>
void appendTransformed( Container& ret,
                        Container& input,
                        Operator operation );

class Slice {

    label partition_{};

    label bottom_{};
    label top_{};

    std::shared_ptr<sliceMap> mapping_{};

    label shift( const label& id ) const;

public:

    // Constructors
    Slice() = default;
    Slice( const label&, const Foam::Offsets&  );

    label partition();

    bool operator()( const label& id ) const;

    bool exist( const label& id ) const;

    label convert( const label& id ) const;

    template< typename Container >
    typename std::enable_if<is_range<Container>::value, Container >::type
    convert( Container& list ) const;

    template< typename Container >
    void append( const Container& );
};

}

template< typename Container >
typename std::enable_if<Foam::is_range<Container>::value, Container >::type
Foam::Slice::convert( Container& list ) const
{
    std::transform( std::begin( list ), std::end( list ),
                    std::begin( list ),
                    [ this ]( const auto& id )
                    { return convert( id ); } );
    return list;
}

template< typename Container >
void Foam::Slice::append( const Container& list ) {
    mapping_->append( list );
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
