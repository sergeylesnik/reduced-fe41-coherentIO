
#ifndef sliceMeshHelper_H
#define sliceMeshHelper_H

#include "label.H"
#include "faceList.H"

#include <set>
#include <vector>
#include <algorithm>

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                       Helper Functions Declaration
\*---------------------------------------------------------------------------*/

template<typename Container>
std::vector<std::pair<label, label> >
generateIndexedPairs( const Container& );

void partitionByFirst( std::vector<std::pair<label, label> >& );

void renumberFace( face&, const std::vector<label> );

void renumberFaces( faceList& faces, const std::vector<label> map );

template<typename Container>
void indexIota( Container&,
                const size_t&,
                const typename Container::value_type& );

template<typename IndexContainer, typename DataContainer>
void indexSort( IndexContainer&, const DataContainer& );

template<typename Container1, typename Container2>
std::vector<std::pair<typename Container1::value_type, typename Container2::value_type> >
zip( const Container1&, const Container2& );

template<typename ValueType, typename UnaryOperation>
auto extractNth( const std::vector<ValueType>&, UnaryOperation );

template<typename IterType, typename ValType>
void findValueExtend( IterType&, IterType&, const IterType&, const ValType& ); 

template<typename Container, typename IndexContainer>
void swapByCurrentIndex( Container&, 
                         IndexContainer&, 
                         typename IndexContainer::value_type&);

template<typename Container, typename IndexContainer>
void swapUntilIndexMatch( Container&, 
                          IndexContainer&,
                          const typename IndexContainer::value_type& );

template<typename Container, typename IndexContainer>
void applyPermutation( Container&, const IndexContainer& );

template<typename Container, typename UnaryPredicate>
std::set<label> pointSubset( const Container&, UnaryPredicate );

template<typename UnaryPredicate>
std::set<label> pointSubset( const faceList&, UnaryPredicate );

template<typename Container>
std::set<label> pointSubset( const Container& );

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include "sliceMeshHelperI.H"

#endif

// ************************************************************************* //
