
#include "Offsets.H"

Foam::Offsets::Offsets( label value, bool parRun ) {
    set( value );
}

void Foam::Offsets::set( Foam::label value ) {
    offsets_[ Foam::Pstream::myProcNo() ] = value;
    Foam::Pstream::gatherList( offsets_ );
    Foam::Pstream::scatterList( offsets_ );

    // If processor does not own corresponding
    // mesh entities. Problem first arised with points.
    for ( label proc = 1; proc < Foam::Pstream::nProcs(); ++proc ) {
        if ( offsets_[ proc ] < offsets_[ proc - 1 ] ) {
            offsets_[ proc ] = offsets_[ proc - 1 ];
        }
    }
}

Foam::label Foam::Offsets::lowerBound( Foam::label myProcNo ) const {
    return ( myProcNo > 0 ) ? offsets_[ myProcNo - 1 ] : 0 ;
}

Foam::label Foam::Offsets::upperBound( Foam::label myProcNo ) const {
    return offsets_[ myProcNo ];
}

Foam::label Foam::Offsets::count( Foam::label myProcNo ) const {
    return upperBound( myProcNo ) - lowerBound( myProcNo );
}
