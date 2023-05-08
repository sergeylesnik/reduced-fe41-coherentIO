
#include "sliceMeshHelper.H"

#include <numeric>

template<typename Container>
std::vector<std::pair<Foam::label, Foam::label> >
Foam::generateIndexedPairs( const Container& input ) {
    std::vector<Foam::label> indices{};
    indexIota( indices, input.size(), 0 );
    std::vector<std::pair<Foam::label, Foam::label> > indexedPairs{};
    indexedPairs.reserve( input.size() );
    std::transform( input.begin(),
                    input.end(),
                    indices.begin(),
                    std::back_inserter( indexedPairs ),
                    []( const auto& inputId, const auto& index )
                    { return std::make_pair( inputId, index ); } );
    return indexedPairs;
}

template<typename Container>
void Foam::indexIota( Container& input,
                      const size_t& size,
                      const typename Container::value_type& start ) {
    input.resize( size );
    std::iota( input.begin(), input.end(), start );
}

template<typename Container1, typename Container2>
std::vector<std::pair<typename Container1::value_type, typename Container2::value_type> >
Foam::zip( const Container1& container1, const Container2& container2 ) {
    typedef typename Container1::value_type type1;
    typedef typename Container2::value_type type2;
    std::vector<std::pair<type1, type2> > zipped{};
    std::transform( container1.begin(), container1.end(), container2.begin(),
                    std::back_inserter( zipped ),
                    [] ( const auto& value1, const auto& value2 )
                    { return std::pair<type1, type2>{value1, value2}; } );
    return zipped;
}


template<typename IndexContainer, typename DataContainer>
void Foam::indexSort( IndexContainer& indices, const DataContainer& data ) {
    std::stable_sort
    (
        indices.begin(),
        indices.end(),
        [&data](const auto& i, const auto& j)
        {
            return data[i] < data[j];
        }
    );
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

template<typename Container, typename UnaryPredicate>
std::set<Foam::label> Foam::pointSubset( const Container& list, UnaryPredicate pred ) {
    std::set<Foam::label> pointIDs{};
    std::copy_if( list.begin(), list.end(),
                  std::inserter( pointIDs, pointIDs.end() ),
                  pred );
    return pointIDs;
}

template<typename UnaryPredicate>
std::set<Foam::label> Foam::pointSubset( const Foam::faceList& faces, UnaryPredicate pred ) {
    std::set<Foam::label> pointIDs{};
    for ( const auto& face : faces ) {
        auto tmp = Foam::pointSubset( face, pred );
        pointIDs.insert( std::begin( tmp ), std::end( tmp ) );
        //std::copy_if( face.begin(), face.end(),
                      //std::inserter( pointIDs, pointIDs.end() ),
                      //pred );
    }
    return pointIDs;
}

template<typename Container>
std::set<Foam::label> Foam::pointSubset( const Container& data ) {
    return Foam::pointSubset( data, [](auto){ return true; } );
}

// ************************************************************************* //
