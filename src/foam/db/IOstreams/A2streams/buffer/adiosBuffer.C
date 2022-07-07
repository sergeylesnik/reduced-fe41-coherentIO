
#include "adiosBuffer.H"

adios2::Dims Foam::toDims( const Foam::labelList& list ) {
    adios2::Dims dims( list.size() );
    std::copy( list.begin(), list.end(), dims.begin() );
    return dims;
}

