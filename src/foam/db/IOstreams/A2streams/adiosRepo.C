
#include "adiosRepo.H"

#include "Pstream.H"
#include "foamString.H"

#include "adiosBuffer.H"

std::unique_ptr<adios2::ADIOS> Foam::adiosRepo::adiosPtr_ = nullptr;
std::unique_ptr< std::map<Foam::string,
                 std::shared_ptr<adios2::IO> > > Foam::adiosRepo::ioMap_ = std::make_unique< std::map<Foam::string,
                                                                               std::shared_ptr<adios2::IO> > >();
std::unique_ptr< std::map<Foam::string,
                 std::shared_ptr<adios2::Engine> > > Foam::adiosRepo::engineMap_ = std::make_unique< std::map<Foam::string,
                                                                                       std::shared_ptr<adios2::Engine> > >();

std::unique_ptr< std::map<Foam::string,
                 std::shared_ptr<Foam::adiosBuffer> > > Foam::adiosRepo::bufferMap_ = std::make_unique< std::map<Foam::string,
                                                                                    std::shared_ptr<Foam::adiosBuffer> > >();

Foam::adiosRepo::adiosRepo() {
    pullADIOS();
}

adios2::ADIOS* Foam::adiosRepo::pullADIOS() {
    if( !adiosPtr_ ) {
        if ( Pstream::parRun() ) {
            adiosPtr_.reset( new adios2::ADIOS( "system/config.xml", MPI_COMM_WORLD ) );
        } else {
            adiosPtr_.reset( new adios2::ADIOS( "system/config.xml" ) );
        }
    }
    return adiosPtr_.get();
}


std::map<Foam::string, std::shared_ptr<adios2::IO> >*
Foam::adiosRepo::get( const std::shared_ptr<adios2::IO>& ) {
    return ioMap_.get();
}

std::map<Foam::string, std::shared_ptr<adios2::Engine> >*
Foam::adiosRepo::get( const std::shared_ptr<adios2::Engine>& ) {
    return engineMap_.get();
}

std::map<Foam::string, std::shared_ptr<Foam::adiosBuffer> >*
Foam::adiosRepo::get( const std::shared_ptr<Foam::adiosBuffer>& ) {
    return bufferMap_.get();
}

void Foam::adiosRepo::push( const Foam::label& input ) {
    boundaryCounter_ = input;
}

void Foam::adiosRepo::close() {
    for ( const auto& enginePair : *engineMap_ ) {
        if ( *(enginePair.second) ) {
            enginePair.second->Close();
        }
    }

    
}
