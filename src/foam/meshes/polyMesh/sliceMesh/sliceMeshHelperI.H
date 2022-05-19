
#include "sliceMeshHelper.H"

#include <numeric>

template<typename Container>
void Foam::indexIota( Container& input,
                      const size_t& size,
                      const typename Container::value_type& start ) {
    input.resize( size );
    std::iota( input.begin(), input.end(), start );
}


template<typename IndexContainer, typename DataContainer>
void Foam::indexSort( IndexContainer& indices, const DataContainer& data ) {
    std::stable_sort( indices.begin(),
                      indices.end(),
                      [ data ]( const auto& i, const auto& j )
                      { return data[i] < data[j]; } );
}


template<typename ValueType, typename UnaryOperation>
auto Foam::extractNth( const std::vector<ValueType>& input, UnaryOperation unary_op ) {
    std::vector<Foam::label> indices; // Only takes containers of labels! TODO
    indices.reserve( input.size() );
    std::transform( input.begin(),
                    input.end(),
                    std::back_inserter( indices ),
                    unary_op );
    return indices;
}


template< typename IterType, typename ValType >
void Foam::findValueExtend( IterType& begin,
                            IterType& end,
                            const IterType& fin,
                            const ValType& value ) {
    begin = std::find_if( begin,
                          fin,
                          [ value ]( const auto& element )
                          { return element == value; } );
    end = std::find_if( begin,
                        fin,
                        [ value ]( const auto& element )
                        { return element != value; } );
}


template<typename Container, typename IndexContainer>
void Foam::swapByCurrentIndex( Container& data, 
                               IndexContainer& indices, 
                               typename IndexContainer::value_type& currentIndex ) {
    using std::swap;
    auto nextIndex = indices[currentIndex];
    swap( data[currentIndex], data[nextIndex] );
    indices[currentIndex] = currentIndex;
    currentIndex = nextIndex;
}


template<typename Container, typename IndexContainer>
void Foam::swapUntilIndexMatch( Container& data, 
                                IndexContainer& indices,
                                const typename IndexContainer::value_type& startingIndex ) {
    auto current = startingIndex;
    while ( startingIndex != indices[current] ) {
        swapByCurrentIndex( data, indices, current );
    }
    indices[current] = current;
}


template<typename Container, typename IndexContainer>
void Foam::applyPermutation( Container& data, const IndexContainer& permutation) {
    typedef typename IndexContainer::value_type size_type;
    IndexContainer indices{ permutation };
    auto size = static_cast<size_type>( indices.size() );
    for ( size_type i = 0; i < size; i++ ) {
        swapUntilIndexMatch( data, indices, i );
    }
}


// ************************************************************************* //
