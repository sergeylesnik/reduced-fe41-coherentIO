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

#include "OFCstream.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

// defineTypeNameAndDebug(Foam::OFCstream, 0);

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template<template<class> class PatchField, class GeoMesh>
Foam::OFCstream<PatchField, GeoMesh>::OFCstream
(
    const fileName& pathname,
    const objectRegistry& registry,
    ios_base::openmode mode,
    streamFormat format,
    versionNumber version,
    compressionType compression
)
:
    OFstream(pathname, mode, format, version, compression),
    pathname_(pathname),
    sliceableMesh_(registry.lookupObject<sliceMesh>(sliceMesh::typeName)),
    fieldId_(-1)
{
    std::cout << "OFCstream I am\n";
}


// * * * * * * * * * * * * * * * * Destructors * * * * * * * * * * * * * * * //

template<template<class> class PatchField, class GeoMesh>
Foam::OFCstream<PatchField, GeoMesh>::~OFCstream()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<template<class> class PatchField, class GeoMesh>
Foam::Ostream&
Foam::OFCstream<PatchField, GeoMesh>::write
(
    const char* data,
    std::streamsize byteSize
)
{
    adiosWritePrimitives
    (
        "fields",
        this->getBlockId(),
        byteSize/sizeof(scalar),
        reinterpret_cast<const scalar*>(data)
    );

    return *this;
}


template<template<class> class PatchField, class GeoMesh>
void Foam::OFCstream<PatchField, GeoMesh>::prepareWrite(label id)
{
    fieldId_ = id;
}

// ************************************************************************* //
