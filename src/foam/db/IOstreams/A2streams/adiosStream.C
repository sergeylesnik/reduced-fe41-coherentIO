
#include "adiosStream.H"

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

void Foam::adiosStream::open( Foam::string&& type ) {
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
