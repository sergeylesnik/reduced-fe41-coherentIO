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

#ifndef adiosWrite_H
#define adiosWrite_H

//#include "IOstream.H"
#include "word.H"
#include "label.H"
#include "fileName.H"
#include "faceList.H"
#include <memory>

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

// Forward declaration
class adiosCore;
class sliceMesh;

/*---------------------------------------------------------------------------*\
                       Class adiosWrite Declaration
\*---------------------------------------------------------------------------*/
class adiosWrite
{

    static adiosCore adiosCore_;

    static std::unique_ptr< sliceMesh > sliceMeshPtr_;

public:

    // Static Member Functions

        //- Set path to the file
        static void setPathName( const fileName& );

        static const fileName& meshPathname();

        static const fileName& dataPathname();

        static void checkFiles();

        static bool dataPresent();

        static bool meshPresent();


    //- Constructor
    adiosWrite() = default;

    //- Destructor
    ~adiosWrite() = default;

    //- Disallow copying
    adiosWrite( adiosWrite const& other ) = delete;
    adiosWrite& operator=( adiosWrite const& other ) = delete;

    // Access

    static std::unique_ptr<sliceMesh>& setSliceMesh
    (
        const labelList&,
        const faceList&,
        const label&
    );

    static void write( const string name,
                       const label shape,
                       const label start,
                       const label count,
                       const scalar* buf );

    static void write( const string name,
                       const label shape,
                       const label start,
                       const label count,
                       const label* buf );

    static void write( const string name,
                       const label shape,
                       const label start,
                       const label count,
                       const char* buf );

    void writeLocalString( const Foam::fileName& varName,
                           const std::string& str,
                           const label size );
};

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
