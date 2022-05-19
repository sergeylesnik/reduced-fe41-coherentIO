/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | foam-extend: Open Source CFD
   \\    /   O peration     | Version:     4.1
    \\  /    A nd           | Web:         http://www.foam-extend.org
     \\/     M anipulation  | For copyright notice see file Copyright
-------------------------------------------------------------------------------
License
    This file is part of foam-extend.

    foam-extend is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by the
    Free Software Foundation, either version 3 of the License, or (at your
    option) any later version.

    foam-extend is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with foam-extend.  If not, see <http://www.gnu.org/licenses/>.

\*---------------------------------------------------------------------------*/

//#include "OSspecific.H"
#include "adiosWrite.H"

//#include "fileName.H"
//#include "faceList.H"

#include "adiosCore.H"
#include "sliceMesh.H"


// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

Foam::adiosCore Foam::adiosWrite::adiosCore_{};

std::unique_ptr< Foam::sliceMesh > Foam::adiosWrite::sliceMeshPtr_ = nullptr;

// * * * * * * * * * * * * Static Member Functions * * * * * * * * * * * * * //

void Foam::adiosWrite::setPathName( const fileName& pathname ) {
    adiosCore_.setPathName( pathname );
}

const Foam::fileName& Foam::adiosWrite::meshPathname() {
    return adiosCore_.meshPathname();
}

const Foam::fileName& Foam::adiosWrite::dataPathname() {
    return adiosCore_.dataPathname();
}

void Foam::adiosWrite::checkFiles() {
    adiosCore_.checkFiles();
}

bool Foam::adiosWrite::dataPresent() {
    return adiosCore_.dataPresent();
}

bool Foam::adiosWrite::meshPresent() {
    return adiosCore_.meshPresent();
}

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //


// * * * * * * * * * * * * * * * * Destructors * * * * * * * * * * * * * * * //


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //


std::unique_ptr<Foam::sliceMesh>&
Foam::adiosWrite::setSliceMesh( const Foam::labelList& faceOwner,
                                const Foam::faceList& allFaces,
                                const Foam::label& nPoints ) {
    sliceMeshPtr_.reset( new Foam::sliceMesh( faceOwner, allFaces, nPoints ) );
    return sliceMeshPtr_;
}

void Foam::adiosWrite::write( const Foam::string blockId,
                              const Foam::label shape,
                              const Foam::label start,
                              const Foam::label count,
                              const Foam::scalar* buf ) {
    adiosCore_.makeVariable<Foam::scalar>( blockId, shape, start, count );
    adiosCore_.write( blockId, buf );
}

void Foam::adiosWrite::write( const Foam::string blockId,
                              const Foam::label shape,
                              const Foam::label start,
                              const Foam::label count,
                              const Foam::label* buf ) {
    adiosCore_.makeVariable<Foam::label>( blockId, shape, start, count );
    adiosCore_.write( blockId, buf );
}

void Foam::adiosWrite::write( const Foam::string blockId,
                              const Foam::label shape,
                              const Foam::label start,
                              const Foam::label count,
                              const char* buf ) {
    adiosCore_.makeVariable<char>( blockId, shape, start, count );
    adiosCore_.write( blockId, buf );
}
void Foam::adiosWrite::writeLocalString( const Foam::fileName& varName,
                                         const std::string& str,
                                         const label size ) {
    write( varName, 0, 0, str.size(), str.data() );
}

// ************************************************************************* //
