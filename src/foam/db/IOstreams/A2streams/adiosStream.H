#ifndef adiosStream_H
#define adiosStream_H

#include "adiosRepo.H"

#include "adiosPaths.H"
#include "variableBuffer.H"

#include "adiosWriting.H"
#include "adiosReading.H"

namespace Foam
{

std::string adiosStreamType( const std::string& id );

template<class adiosFactory> void adiosBeginStep( const Foam::string& );
template<class adiosFactory> void adiosEndStep( const Foam::string& );

template<typename Container>
void adiosReadToContainer( const string&, const string&, Container& );
template<typename Container>
void adiosReadToContainer( const string&, const string&, Container&, const labelList&, const labelList& );
template<typename Container>
void adiosReadToContainer( const string&, const string&, Container&, const label&, const label& );



class adiosStream {

    virtual void v_open() = 0;

protected:

    adiosPaths paths_{};
    adiosRepo repo_{};
    Foam::string type_;
    std::shared_ptr<adios2::IO> ioPtr_{ nullptr };
    std::shared_ptr<adios2::Engine> enginePtr_{ nullptr };
    std::shared_ptr<adiosBuffer> bufferPtr_{ nullptr };

    void setPath( const Foam::string& );

    template<typename BufferType>
    label readingBuffer( const Foam::string& blockId );

    template<typename BufferType>
    label readingBuffer( const Foam::string& blockId,
                         const labelList& start,
                         const labelList& count );

    template<typename BufferType>
    void writingBuffer( const Foam::string& blockId,
                        const Foam::labelList& shape,
                        const Foam::labelList& start,
                        const Foam::labelList& count );

public:

    virtual ~adiosStream() = default;

    void open( const Foam::string& );

    void beginStep();

    void endStep();

    // Reading local array
    template<class DataType>
    typename std::enable_if<!std::is_const<DataType>::value, void>::type
    transfer( const string& blockId, DataType* data );

    // Reading local array
    template<class ContainerType>
    typename std::enable_if<!std::is_const<ContainerType>::value, void>::type
    transfer( const string& blockId, ContainerType& data );

    // Reading global array
    template<class DataType>
    typename std::enable_if<!std::is_const<DataType>::value, void>::type
    transfer( const string& blockId,
              DataType* data,
              const labelList& start,
              const labelList& count );

    // Reading global array
    template<class ContainerType>
    typename std::enable_if<!std::is_const<ContainerType>::value, void>::type
    transfer( const string& blockId,
              ContainerType& data,
              const labelList& start,
              const labelList& count );

    // Writing
    template<class DataType>
    void transfer( const Foam::string& blockId,
                   const Foam::label& shape,
                   const Foam::label& start,
                   const Foam::label& count,
                   const DataType* buf );

    // Writing
    template<class DataType>
    void transfer( const Foam::string& blockId,
                   const Foam::labelList& shape,
                   const Foam::labelList& start,
                   const Foam::labelList& count,
                   const DataType* buf );

    void close();
};

}

template<typename BufferType>
Foam::label Foam::adiosStream::readingBuffer( const Foam::string& blockId ) {
    std::shared_ptr<adiosBuffer> bufferPtr{ nullptr };
    repo_.pull( bufferPtr, blockId );
    if ( !bufferPtr && ioPtr_.get() && enginePtr_.get() ) {
        bufferPtr = std::make_shared<BufferType>( ioPtr_.get(), enginePtr_.get(), blockId );
        repo_.push( bufferPtr, blockId );
    }
    bufferPtr_ = bufferPtr;
    return bufferPtr_->size();
}

template<typename BufferType>
Foam::label Foam::adiosStream::readingBuffer( const Foam::string& blockId,
                                              const Foam::labelList& start,
                                              const Foam::labelList& count ) {
    std::shared_ptr<adiosBuffer> bufferPtr{ nullptr };
    repo_.pull( bufferPtr, blockId );
    if ( !bufferPtr && ioPtr_.get() && enginePtr_.get() ) {
        bufferPtr = std::make_shared<BufferType>( ioPtr_.get(), enginePtr_.get(),
                                                  blockId, start, count );
        repo_.push( bufferPtr, blockId );
    }
    bufferPtr_ = bufferPtr;
    return bufferPtr_->size();
}


template<typename BufferType>
void Foam::adiosStream::writingBuffer( const Foam::string& blockId,
                                       const Foam::labelList& shape,
                                       const Foam::labelList& start,
                                       const Foam::labelList& count ) {
    std::shared_ptr<adiosBuffer> bufferPtr{ nullptr };
    repo_.pull( bufferPtr, blockId );
    if ( !bufferPtr && ioPtr_.get() && enginePtr_.get() ) {
        bufferPtr = std::make_shared<BufferType>( ioPtr_.get(), enginePtr_.get(),
                                                  blockId, shape, start, count );
        repo_.push( bufferPtr, blockId );
    }
    bufferPtr_ = bufferPtr;
}

// Reading local array
template<class DataType>
typename std::enable_if<!std::is_const<DataType>::value, void>::type
Foam::adiosStream::transfer( const Foam::string& blockId, DataType* data ) {
    readingBuffer<Foam::variableBuffer<DataType> >( blockId );
    if ( bufferPtr_ ) {
        bufferPtr_->transfer( enginePtr_.get(), data );
    }
}

// Reading local array
template<class ContainerType>
typename std::enable_if<!std::is_const<ContainerType>::value, void>::type
Foam::adiosStream::transfer( const Foam::string& blockId, ContainerType& container ) {
    typedef typename ContainerType::value_type DataType;
    auto size = readingBuffer<Foam::variableBuffer<DataType> >( blockId );
    container.resize( size );
    if ( bufferPtr_ ) {
        bufferPtr_->transfer( enginePtr_.get(), container.data() );
    }
}

// Reading global array
template<class DataType>
typename std::enable_if<!std::is_const<DataType>::value, void>::type
Foam::adiosStream::transfer( const Foam::string& blockId,
                             DataType* data,
                             const Foam::labelList& start,
                             const Foam::labelList& count ) {
    readingBuffer<Foam::variableBuffer<DataType> >( blockId, start, count );
    if ( bufferPtr_ ) {
        bufferPtr_->transfer( enginePtr_.get(), data );
    }
}

// Reading global array
template<class ContainerType>
typename std::enable_if<!std::is_const<ContainerType>::value, void>::type
Foam::adiosStream::transfer( const Foam::string& blockId,
                             ContainerType& container,
                             const Foam::labelList& start,
                             const Foam::labelList& count ) {
    typedef typename ContainerType::value_type DataType;
    readingBuffer<Foam::variableBuffer<DataType> >( blockId, start, count );
    container.resize( count[0] );
    if ( bufferPtr_ ) {
        bufferPtr_->transfer( enginePtr_.get(), container.data() );
    }
}

template<class DataType>
void Foam::adiosStream::transfer( const Foam::string& blockId,
                                  const Foam::label& iShape,
                                  const Foam::label& iStart,
                                  const Foam::label& iCount,
                                  const DataType* data ) {
    Foam::labelList shape{1}; shape[0] = iShape;
    Foam::labelList start{1}; start[0] = iStart;
    Foam::labelList count{1}; count[0] = iCount;
    transfer( blockId, shape, start, count, data );
}

template<class DataType>
void Foam::adiosStream::transfer( const Foam::string& blockId,
                                  const Foam::labelList& shape,
                                  const Foam::labelList& start,
                                  const Foam::labelList& count,
                                  const DataType* data ) {
    writingBuffer<Foam::variableBuffer<DataType> >( blockId, shape, start, count );
    if ( bufferPtr_ ) {
        bufferPtr_->transfer( enginePtr_.get(), data );
    }
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
                                 Container& container ) {
    auto adiosStreamPtr = adiosReading{}.createStream();
    adiosStreamPtr->open( std::move( type ) );
    adiosStreamPtr->transfer( blockId, container );
    adiosStreamPtr->endStep();
}

template <typename Container>
void Foam::adiosReadToContainer( const Foam::string& type,
                                 const Foam::string& blockId,
                                 Container& container,
                                 const Foam::labelList& start,
                                 const Foam::labelList& count ) {
    auto adiosStreamPtr = adiosReading{}.createStream();
    adiosStreamPtr->open( std::move( type ) );
    adiosStreamPtr->transfer( blockId, container, start, count );
    adiosStreamPtr->endStep();
}

template <typename Container>
void Foam::adiosReadToContainer( const Foam::string& type,
                                 const Foam::string& blockId,
                                 Container& container,
                                 const Foam::label& start,
                                 const Foam::label& count ) {
    Foam::labelList listedStart{1}; listedStart[0] = start;
    Foam::labelList listedCount{1}; listedCount[0] = count;
    Foam::adiosReadToContainer( type, blockId, container, listedStart, listedCount);
}

#endif
