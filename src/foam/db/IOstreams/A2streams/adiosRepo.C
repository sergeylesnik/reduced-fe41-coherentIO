
#include "adiosRepo.H"

#include "adios2.h"

#include "Pstream.H"
#include "foamString.H"

#include "adiosBuffer.H"

struct Foam::adiosRepo::Impl {

    using ADIOS_uPtr = std::unique_ptr<adios2::ADIOS>;
    using IO_map = std::map<Foam::string, std::shared_ptr<adios2::IO> >;
    using IO_map_uPtr = std::unique_ptr<IO_map>;
    using Engine_map = std::map<Foam::string, std::shared_ptr<adios2::Engine> >;
    using Engine_map_uPtr = std::unique_ptr<Engine_map>;
    using Buffer_map = std::map<Foam::string, std::shared_ptr<adiosBuffer> >;
    using Buffer_map_uPtr = std::unique_ptr<Buffer_map>;

    Impl() {
        if( !adiosPtr_ ) {
            if ( Pstream::parRun() ) {
                adiosPtr_.reset( new adios2::ADIOS( "system/config.xml", MPI_COMM_WORLD ) );
            } else {
                adiosPtr_.reset( new adios2::ADIOS( "system/config.xml" ) );
            }
        }
    }

    static ADIOS_uPtr adiosPtr_;
    static std::unique_ptr<IO_map> ioMap_;
    static std::unique_ptr<Engine_map> engineMap_;
    static std::unique_ptr<Buffer_map> bufferMap_;

};

Foam::adiosRepo::Impl::ADIOS_uPtr
Foam::adiosRepo::Impl::adiosPtr_ = nullptr;
Foam::adiosRepo::Impl::IO_map_uPtr
Foam::adiosRepo::Impl::ioMap_ = std::make_unique<Foam::adiosRepo::Impl::IO_map>();
Foam::adiosRepo::Impl::Engine_map_uPtr
Foam::adiosRepo::Impl::engineMap_ = std::make_unique<Foam::adiosRepo::Impl::Engine_map>();
Foam::adiosRepo::Impl::Buffer_map_uPtr
Foam::adiosRepo::Impl::bufferMap_ = std::make_unique<Foam::adiosRepo::Impl::Buffer_map>();

std::unique_ptr<Foam::adiosRepo::Impl>
Foam::adiosRepo::pimpl_ = std::make_unique<Foam::adiosRepo::Impl>();

Foam::adiosRepo::adiosRepo() = default;
Foam::adiosRepo::~adiosRepo() = default;

adios2::ADIOS*
Foam::adiosRepo::pullADIOS() {
    return pimpl_->adiosPtr_.get();
}

std::map<Foam::string, std::shared_ptr<adios2::IO> >*
Foam::adiosRepo::get( const std::shared_ptr<adios2::IO>& ) {
    return pimpl_->ioMap_.get();
}

std::map<Foam::string, std::shared_ptr<adios2::Engine> >*
Foam::adiosRepo::get( const std::shared_ptr<adios2::Engine>& ) {
    return pimpl_->engineMap_.get();
}

std::map<Foam::string, std::shared_ptr<Foam::adiosBuffer> >*
Foam::adiosRepo::get( const std::shared_ptr<Foam::adiosBuffer>& ) {
    return pimpl_->bufferMap_.get();
}

void Foam::adiosRepo::push( const Foam::label& input ) {
    boundaryCounter_ = input;
}

void Foam::adiosRepo::close() {
    for ( const auto& enginePair : *(pimpl_->engineMap_) ) {
        if ( *(enginePair.second) ) {
            enginePair.second->Close();
        }
    }

    
}
