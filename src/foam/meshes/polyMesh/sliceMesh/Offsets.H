#ifndef Offsets_H
#define Offsets_H

#include "Pstream.H"

namespace Foam
{

class Offsets {

    labelList offsets_{ labelList(Pstream::nProcs(), 0) };

    void do_reduce();

public:

    Offsets() = default;

    explicit Offsets( label value,
                      bool parRun = Pstream::parRun(),
                      bool reduce = false );

    void set( label value, 
              bool reduce = false );

    label lowerBound( label ) const;
    label upperBound( label ) const;
    label count( label ) const;

};

}

#endif
