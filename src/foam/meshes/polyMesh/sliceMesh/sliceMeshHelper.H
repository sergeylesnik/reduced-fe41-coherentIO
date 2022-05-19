
#ifndef sliceMeshHelper_H
#define sliceMeshHelper_H

#include "label.H"
#include "faceList.H"

#include <vector>
#include <algorithm>

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                       Helper Functions Declaration
\*---------------------------------------------------------------------------*/

std::vector<std::pair<label, label> >
generateIndexedPairs( const std::vector<label>& );

void partitionByFirst( std::vector<std::pair<label, label> >& );

void renumberFace( face&, const std::vector<label> );

void renumberFaces( faceList& faces, const std::vector<label> map );

template<typename Container>
void indexIota( Container&,
                const size_t&,
                const typename Container::value_type& );

template<typename IndexContainer, typename DataContainer>
void indexSort( IndexContainer&, const DataContainer& );

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

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include "sliceMeshHelperI.H"

#endif

// ************************************************************************* //
