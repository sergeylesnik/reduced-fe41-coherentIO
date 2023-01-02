

#include "adiosFileStream.H"
#include "adiosFeatures.H"

#include "adiosStreamImpl.H"

#include "foamString.H"

Foam::adiosFileStream::adiosFileStream( std::unique_ptr<adiosFeatures>& fileFeatures )
    : adiosFile_{ std::move( fileFeatures ) }
{}

void Foam::adiosFileStream::v_open() {
    ioPtr_ = adiosFile_->createIO( repo_.pullADIOS() );
    enginePtr_ = adiosFile_->createEngine( ioPtr_.get(), paths_.getPathName() );
}

