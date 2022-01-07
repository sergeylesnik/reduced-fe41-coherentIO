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

#include "Engine.h"
#include "word.H"
#include "label.H"
#include "fileName.H"
#include <memory>
#include "adiosCore.H"
#include "adios2.h"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

//Forward declaration
class adiosCore;

/*---------------------------------------------------------------------------*\
                       Class adiosWrite Declaration
\*---------------------------------------------------------------------------*/
class adiosWrite
:
public adiosCore
{
    static std::unique_ptr<adios2::IO> ioWritePtr_;

    static std::unique_ptr<adios2::Engine> enginePtr_;

    static fileName pathname_;

    std::unique_ptr<adios2::Variable<double> > variablePtr_;

public:

    // Static Member Functions

        //- Close adios engine if existent
        static void beginStep();

        //- Close adios engine if existent
        static void endStep();

        //- Close adios engine if existent
        static void close();


    //- Constructor
    adiosWrite(const fileName& pathname);

    //- Destructor
    ~adiosWrite();

    // Access

        fileName pathname();

        static std::unique_ptr<adios2::IO>& ioWritePtr();

        static std::unique_ptr<adios2::Engine>& enginePtr();

    void open();

    void defineVariable
    (
        const string name,
        const label shape,
        const label start,
        const label count,
        const bool constantDims = false
    );

    void put(const double* buf);

    void performPuts();

    void write
    (
        const string name,
        const label shape,
        const label start,
        const label count,
        const double* buf
    );

    void writeLocalString
    (
        const Foam::fileName& varName,
        const std::string& str,
        const label size
    );
};

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
