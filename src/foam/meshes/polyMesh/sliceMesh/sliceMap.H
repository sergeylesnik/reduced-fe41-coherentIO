
#ifndef sliceMap_H
#define sliceMap_H

#include "label.H"

#include <map>
#include <algorithm>

namespace Foam
{

class sliceMap {

    std::map<label, label> mapping_{};

    const label numNativeEntities_{};

public:

    // Constructor
    sliceMap() = default;
    sliceMap( const label& );

    template< typename Container >
    void append( const Container& );

    label operator[]( const label& );

    bool exist( const label& id ) { return mapping_.count( id ) == 1; };

};

}

template< typename Container >
void Foam::sliceMap::append( const Container& list ) {
    Foam::label currId = numNativeEntities_ + mapping_.size();
    std::transform( std::begin( list ), std::end( list ),
                    std::inserter( mapping_, std::end( mapping_ ) ),
                    [ &currId ] ( const auto& id ) mutable {
                        auto ret = std::pair<label, label>( { id, currId } );
                        ++currId;
                        return ret;
                    } );
}

#endif
