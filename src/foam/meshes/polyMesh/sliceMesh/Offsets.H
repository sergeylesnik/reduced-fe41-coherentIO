#ifndef Offsets_H
#define Offsets_H

#include "Pstream.H"

namespace Foam
{

class Offsets {

    labelList offsets_{ Pstream::nProcs(), 0 };

public:

    Offsets() = default;

    explicit Offsets( label value, bool parRun = Pstream::parRun() );

    void set( label value );

    label lowerBound( label ) const;
    label upperBound( label ) const;
    label count( label ) const;

};

}

#endif
