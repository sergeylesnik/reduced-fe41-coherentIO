#ifndef adiosStream_H
#define adiosStream_H

#include "labelList.H"

#include "adiosRepo.H"

#include "adiosPaths.H"

#include "adiosWriting.H"
#include "adiosReading.H"

namespace Foam
{

std::string adiosStreamType( const std::string& id );

template<class adiosFactory> void adiosBeginStep( const Foam::string& );
template<class adiosFactory> void adiosEndStep( const Foam::string& );

template<typename Container>
void adiosReadToContainer( const Foam::string& type,
                           const Foam::string& blockId,
                           Container& container,
                           const labelList& start = {},
                           const labelList& count = {});


class adiosStream {

    virtual void v_open() = 0;

protected:

    class Impl;
    std::unique_ptr<Impl> pimpl_{ nullptr };

    adiosPaths paths_{};
    adiosRepo repo_{};
    Foam::string type_{};
    std::shared_ptr<adios2::IO> ioPtr_{ nullptr };
    std::shared_ptr<adios2::Engine> enginePtr_{ nullptr };

    void setPath( const Foam::string& );

public:

    adiosStream();
    virtual ~adiosStream();

    void open( const Foam::string& );

    void beginStep();

    void endStep();

    // Reading local/global array
    void transfer( const string& blockId,
                   scalar* data,
                   const labelList& start = {},
                   const labelList& count = {} );
    void transfer( const string& blockId,
                   label* data,
                   const labelList& start = {},
                   const labelList& count = {} );
    void transfer( const string& blockId,
                   char* data,
                   const labelList& start = {},
                   const labelList& count = {} );

    label getBufferSize( const Foam::string& blockId, const scalar* const data );
    label getBufferSize( const Foam::string& blockId, const label* const data );
    label getBufferSize( const Foam::string& blockId, const char* const data );

    // Reading local/global array
    template<class ContainerType>
    typename std::enable_if<!std::is_const<ContainerType>::value, void>::type
    transfer( const string& blockId,
              ContainerType& data,
              const labelList& start = {},
              const labelList& count = {} );

    // Writing
    void transfer( const Foam::string& blockId,
                   const Foam::labelList& shape,
                   const Foam::labelList& start,
                   const Foam::labelList& count,
                   const scalar* buf,
                   const labelList& mapping = {},
                   const bool masked = false );
    void transfer( const Foam::string& blockId,
                   const Foam::labelList& shape,
                   const Foam::labelList& start,
                   const Foam::labelList& count,
                   const label* buf,
                   const labelList& mapping = {},
                   const bool masked = false );
    void transfer( const Foam::string& blockId,
                   const Foam::labelList& shape,
                   const Foam::labelList& start,
                   const Foam::labelList& count,
                   const char* buf,
                   const labelList& mapping = {},
                   const bool masked = false );

    void close();
};

}

// Reading local/global array
template<class ContainerType>
typename std::enable_if<!std::is_const<ContainerType>::value, void>::type
Foam::adiosStream::transfer( const Foam::string& blockId,
                             ContainerType& container,
                             const Foam::labelList& start,
                             const Foam::labelList& count ) {
    if ( start.empty() && count.empty() ) {
        auto size = getBufferSize( blockId, container.data() );
        container.resize( size );
    } else {
        container.resize( count[ 0 ] );
    }
    transfer( blockId, container.data(), start, count );
}

template <class adiosFactory>
void Foam::adiosBeginStep( const Foam::string& type ) {
    auto adiosStreamPtr = adiosFactory{}.createStream();
    adiosStreamPtr->open( type );
    adiosStreamPtr->beginStep();
}

template <class adiosFactory>
void Foam::adiosEndStep( const Foam::string& type ) {
    auto adiosStreamPtr = adiosFactory{}.createStream();
    adiosStreamPtr->open( type );
    adiosStreamPtr->endStep();
}

template <typename Container>
void Foam::adiosReadToContainer( const Foam::string& type,
                                 const Foam::string& blockId,
                                 Container& container,
                                 const Foam::labelList& start ,
                                 const Foam::labelList& count ) {
    auto adiosStreamPtr = adiosReading{}.createStream();
    adiosStreamPtr->open( std::move( type ) );
    adiosStreamPtr->transfer( blockId, container, start, count );
    adiosStreamPtr->endStep();
}

#endif
