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

Class
    Foam::OFCstream

Description
    Output to file stream for coherent mesh and fields.

SourceFiles
    OFCstream.C

\*---------------------------------------------------------------------------*/

#ifndef OFCstream_H
#define OFCstream_H

#include "OFstream.H"
#include "sliceMesh.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                          Class OFCstream Declaration
\*---------------------------------------------------------------------------*/

template<template<class> class PatchField, class GeoMesh>
class OFCstream
:
    public OFstream
{
    // Private data

        fileName pathname_;
        const sliceMesh& sliceableMesh_;

        //- Identifier for the field to be written
        label fieldId_;

public:

    // Declare name of the class and its debug switch
    TypeName("OFCstream");


    // Constructors

        //- Construct from pathname
        OFCstream
        (
            const fileName& pathname,
            const objectRegistry& registry,
            ios_base::openmode mode = ios_base::out|ios_base::trunc,
            streamFormat format = ASCII,
            versionNumber version = currentVersion,
            compressionType compression = UNCOMPRESSED
        );


    // Destructor

        ~OFCstream();


    // Member functions

        //- Write field with the scalar primitive type
        virtual Ostream& write(const char*, std::streamsize);

        //- Prepare the stream to write a certain field
        void prepareWrite(label);

};



} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#ifdef NoRepository
#   include "OFCstream.C"
#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
