
#ifndef adiosVariableData_H
#define adiosVariableData_H

#include <memory>

#include "label.H"
#include "adios2.h"

namespace Foam
{

// Forward declaration
template < typename T > struct adiosVariable;
template < typename T > struct adiosSpan;

struct adiosVariableData
{
  adiosVariableData() = default; 
  virtual ~adiosVariableData() = default;

  virtual void createSpan( adios2::Engine* engine ) {};

  template < typename T > 
  static std::unique_ptr<adiosVariable<T> >
  makeVariable( adios2::IO* const io,
                const Foam::string blockId,
                const Foam::label count )
  {
    return std::make_unique<adiosVariable<T> >(io, blockId, count);
  }

  template < typename T > 
  static std::unique_ptr<adiosSpan<T> >
  makeVariableWithSpan( adios2::IO* const io, 
                        adios2::Engine* const engine,
                        const Foam::string blockId,
                        const Foam::label count )
  { 
    return std::make_unique<adiosSpan<T> >(io, engine, blockId, count);
  }

  template < typename T > 
  static std::unique_ptr<adiosVariable<T> >
  getVariable( adios2::IO* const io,
                const Foam::string blockId )
  {
    return std::make_unique<adiosVariable<T> >(io, blockId);
  }
};

template< typename T >
struct adiosVariable : public adiosVariableData
{
  adios2::Variable<T> variable_;

  adiosVariable( adios2::IO* const io,
                 const Foam::string blockId,
                 const Foam::label count )
  {
    variable_ = io->DefineVariable<T>
                    (
                      blockId,
                      {},
                      {},
                      {static_cast<size_t>(count)},
                      adios2::ConstantDims
                    );
    //auto blocksInfo = enginePtr()->BlocksInfo(variable_, 1);
    //std::cout << "\n Dims = " << blocksInfo.size() << "\n";
  }

  adiosVariable( adios2::IO* const io,
                 const Foam::string blockId )
  {
    variable_ = io->InquireVariable<T>( blockId );
    //auto blocksInfo = enginePtr()->BlocksInfo(variable_, 1);
    //std::cout << "\n Retrieved dims = " << blocksInfo.size() << "\n";
  }
};

template< typename T >
struct adiosSpan : public adiosVariableData
{
  adios2::Variable<T> variable_;
  std::unique_ptr<typename adios2::Variable<T>::Span > span_{ nullptr };

  adiosSpan( adios2::IO* io,
             adios2::Engine* engine,
             const Foam::string blockId,
             const Foam::label count )
  {
    variable_ = io->DefineVariable<T>
                    (
                      blockId,
                      {},
                      {},
                      {static_cast<size_t>(count)},
                      adios2::ConstantDims
                    );
    //span_.reset( new typename adios2::Variable<T>::Span{ engine->Put( variable_ ) } );
    //engine->Put( variable_ );
  }

  void createSpan( adios2::Engine* engine ) override
  { auto span = engine->Put(variable_); }
};

}

#endif
