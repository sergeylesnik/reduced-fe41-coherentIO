
#include "adiosOutputFeatures.H"

#include "adios2.h"
#include "IO.h"
#include "Engine.h"

#include "adiosRepo.H"

#include "fileName.H"

std::shared_ptr<adios2::IO>
Foam::adiosOutputFeatures::createIO( adios2::ADIOS* const adiosPtr ) {
    adiosRepo repo{};
    std::shared_ptr<adios2::IO> ioPtr{ nullptr };
    repo.pull( ioPtr, "write" );
    if ( !ioPtr ) {
        ioPtr = std::make_shared<adios2::IO>( adiosPtr->DeclareIO( "write" ) );
        ioPtr->SetEngine( "BP4" );
        repo.push( ioPtr, "write" );
    }
    return ioPtr;
}

std::shared_ptr<adios2::Engine>
Foam::adiosOutputFeatures::createEngine( adios2::IO* const ioPtr, const Foam::fileName& path ) {
    adiosRepo repo{};
    std::shared_ptr<adios2::Engine> enginePtr{ nullptr };
    auto size = path.length();
    repo.pull( enginePtr, "write"+path );
    if ( !enginePtr ) {
        enginePtr = std::make_shared<adios2::Engine>( ioPtr->Open( path, adios2::Mode::Append ) );
        repo.push( enginePtr,  "write"+path( size ) );
    }
    return enginePtr;
}

