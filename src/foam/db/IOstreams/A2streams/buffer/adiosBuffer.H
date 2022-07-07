
#ifndef adiosBuffer_H
#define adiosBuffer_H

#include "adios2.h"

#include <memory>

#include "label.H"
#include "labelList.H"
#include "vector.H"
#include "messageStream.H" // FatalError()

namespace Foam
{

adios2::Dims toDims( const Foam::labelList& list );

class adiosBuffer
{
  virtual void v_transfer( adios2::Engine*, const char*) {};
  virtual void v_transfer( adios2::Engine*, char*) {};
  virtual void v_transfer( adios2::Engine*, const label*) {};
  virtual void v_transfer( adios2::Engine*, label*) {};
  virtual void v_transfer( adios2::Engine*, const scalar*) {};
  virtual void v_transfer( adios2::Engine*, scalar*) {};

public:

  adiosBuffer() = default;

  virtual ~adiosBuffer() = default;

  virtual std::shared_ptr<adiosBuffer> clone( adios2::IO* io,
                                              adios2::Engine* engine,
                                              const Foam::string blockId,
                                              const Foam::labelList& shape,
                                              const Foam::labelList& start,
                                              const Foam::labelList& count ) = 0;

  virtual label size() = 0;

  template<typename DataType>
  void transfer( adios2::Engine* engine, DataType* const data ) {
    if ( !engine ) {
        FatalErrorIn("adiosBuffer::transfer( adios2::Engine*, DataType* const ) engine is nullptr")
            << abort(FatalError);
    }
    if ( !data ) {
        FatalErrorIn("adiosBuffer::transfer( adios2::Engine*, DataType* const ) data is nullptr")
            << abort(FatalError);
    }
    v_transfer( engine, data );
  }
};

}

#endif
