
#include "adiosStream.H"

#include "adiosStreamImpl.H"

Foam::adiosStream::adiosStream()
    : pimpl_{ new Impl() }
    , paths_{}
    , repo_{}
    , type_{}
    , ioPtr_{ nullptr }
    , enginePtr_{ nullptr } {}

Foam::adiosStream::~adiosStream() = default;

std::string Foam::adiosStreamType( const std::string& id ) {
    std::string type = "fields";
    if ( ( id.find("polyMesh") != std::string::npos )
         ||
         ( id.find("region") != std::string::npos ) ) {
        type = "mesh";
    }
    return type;
}


void Foam::adiosStream::setPath( const Foam::string& type ) {
    if ( type == "mesh" ) {
        //paths_.setPathName( "constant/polyMesh.bp" );
        // Enable this later
        paths_.setPathName( paths_.meshPathname() );
    } else if ( type == "fields" ) {
        //paths_.setPathName( "fields.bp" );
        // Enable this later
        paths_.setPathName( paths_.dataPathname() );
    }
}

void Foam::adiosStream::open( const Foam::string& type ) {
    type_ = type;
    setPath( type );
    v_open();
}

void Foam::adiosStream::beginStep() {
    enginePtr_->BeginStep();
}

void Foam::adiosStream::endStep() {
    enginePtr_->EndStep();
}

void Foam::adiosStream::close() {
    enginePtr_->Close();
    repo_.remove( enginePtr_, type_ );
}

Foam::label Foam::adiosStream::getBufferSize( const Foam::string& blockId,
                                              const Foam::scalar* const data ) {
    return pimpl_->readingBuffer<Foam::variableBuffer<Foam::scalar> >( ioPtr_.get(),
                                                                       enginePtr_.get(),
                                                                       blockId );
}
Foam::label Foam::adiosStream::getBufferSize( const Foam::string& blockId,
                                              const Foam::label* const data ) {
    return pimpl_->readingBuffer<Foam::variableBuffer<Foam::label> >( ioPtr_.get(),
                                                                      enginePtr_.get(),
                                                                      blockId );
}
Foam::label Foam::adiosStream::getBufferSize( const Foam::string& blockId,
                                              const char* const data ) {
    return pimpl_->readingBuffer<Foam::variableBuffer<char> >( ioPtr_.get(),
                                                               enginePtr_.get(),
                                                               blockId );
}

// Reading
void Foam::adiosStream::transfer( const Foam::string& blockId,
                                  scalar* data,
                                  const Foam::labelList& start,
                                  const Foam::labelList& count ) {
    pimpl_->transfer( ioPtr_.get(), enginePtr_.get(), blockId, data, start, count );
}

void Foam::adiosStream::transfer( const Foam::string& blockId,
                                  label* data,
                                  const Foam::labelList& start,
                                  const Foam::labelList& count ) {
    pimpl_->transfer( ioPtr_.get(), enginePtr_.get(), blockId, data, start, count );
}

void Foam::adiosStream::transfer( const Foam::string& blockId,
                                  char* data,
                                  const Foam::labelList& start,
                                  const Foam::labelList& count ) {
    pimpl_->transfer( ioPtr_.get(), enginePtr_.get(), blockId, data, start, count );
}

// Writing
void Foam::adiosStream::transfer( const Foam::string& blockId,
                                  const Foam::labelList& shape,
                                  const Foam::labelList& start,
                                  const Foam::labelList& count,
                                  const scalar* data ) {
    pimpl_->transfer( ioPtr_.get(), enginePtr_.get(), blockId, shape, start, count, data );
}

void Foam::adiosStream::transfer( const Foam::string& blockId,
                                  const Foam::labelList& shape,
                                  const Foam::labelList& start,
                                  const Foam::labelList& count,
                                  const label* data ) {
    pimpl_->transfer( ioPtr_.get(), enginePtr_.get(), blockId, shape, start, count, data );
}

void Foam::adiosStream::transfer( const Foam::string& blockId,
                                  const Foam::labelList& shape,
                                  const Foam::labelList& start,
                                  const Foam::labelList& count,
                                  const char* data ) {
    pimpl_->transfer( ioPtr_.get(), enginePtr_.get(), blockId, shape, start, count, data );
}

