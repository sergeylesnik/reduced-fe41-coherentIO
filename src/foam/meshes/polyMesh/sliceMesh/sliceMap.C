
#include "sliceMap.H"

Foam::sliceMap::sliceMap( const Foam::label& numNativeEntities ) 
    : mapping_{}
    , numNativeEntities_{ numNativeEntities } {}

Foam::label Foam::sliceMap::operator[]( const Foam::label& id ) { 
    return mapping_[ id ]; 
}
