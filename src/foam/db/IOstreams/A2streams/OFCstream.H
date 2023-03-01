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
#include "dictionary.H"
#include "fieldDataEntry.H"

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
        sliceMesh& sliceableMesh_;

        //- Dictionary holding both entries for the ASCII file and pointer,
        //  size data for the BLOBs.
        dictionary dict_;

        //- Pointer to the (sub-)dictionary the ofstream currently writes to
        dictionary* currentSubDictPtr_;

        //- The last keyword seen on the stream
        keyType currentKeyword_;

        //- Entry counter used for the formatting entries
        label currentEntryI_;

        //- The last compound token senn on the stream
        token currentCompoundToken_;

        // Pointers to data chunks contributing to a single array staged until
        // all data is gathered and the consolidated data array is filled
        scalarList consolidatedData_;


    // Private member functions

        void gatherFieldDataEntries
        (
            dictionary&,
            DynamicList<fieldDataEntry*>&
        );

        void removeProcPatchesFromDict();
        void writeGlobalGeometricField();

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

        virtual ~OFCstream();


    // Member functions

        //- Get variable id without "processor.*" if writing a global field
        virtual string getBlockId() const;

        //- Get variable id without "processor.*"
        string getGlobalId() const;

        virtual Ostream& writeKeyword(const keyType&);
        virtual Ostream& write(const token&);
        virtual Ostream& write(const word&);
        virtual word incrBlock(const word);
        virtual void decrBlock();
        virtual Ostream& parwrite(const char*, std::streamsize, label);

        //- Write global field as a single variable
        void writeGlobalField
        (
            const label globalSize,
            const label offset,
            const label localSize,
            const scalar* data,
            const string name
        ) const;

        //- Put current stream buffer to a dictionary as an entry
        void moveStreamBufferToDict();

        //- Write the dictionary manually for correct formatting
        void writeDict(Ostream& os, const dictionary& dict, bool subDict) const;
};


} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#ifdef NoRepository
#   include "OFCstream.C"
#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
