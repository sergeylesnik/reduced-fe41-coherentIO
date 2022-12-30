
#include "adiosInputFeatures.H"

#include "adios2.h"
#include "IO.h"
#include "Engine.h"

#include "adiosRepo.H"

#include "fileName.H"

std::shared_ptr<adios2::IO> Foam::adiosInputFeatures::createIO( adios2::ADIOS* const adiosPtr ) {
    adiosRepo repo{};
    std::shared_ptr<adios2::IO> ioPtr{ nullptr };
    repo.pull( ioPtr, "read" );
    if ( !ioPtr ) {
        ioPtr = std::make_shared<adios2::IO>( adiosPtr->DeclareIO( "read" ) );
        ioPtr->SetEngine( "BP4" );
        repo.push( ioPtr, "read" );
    }
    return ioPtr;
}

std::shared_ptr<adios2::Engine> Foam::adiosInputFeatures::createEngine( adios2::IO* const ioPtr, const Foam::fileName& path ) {
    adiosRepo repo{};
    std::shared_ptr<adios2::Engine> enginePtr{ nullptr };
    auto size = path.length();
    repo.pull( enginePtr, "read"+path( size ) );
    if ( !enginePtr ) {
        enginePtr = std::make_shared<adios2::Engine>( ioPtr->Open( path, adios2::Mode::Read ) );
        repo.push( enginePtr, "read"+path( size ) );
    }
    return enginePtr;
}

