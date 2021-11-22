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

#ifndef adiosCore_H
#define adiosCore_H

#include "adios2.h"

#include "word.H"
#include "labelList.H"
#include <memory>
#include <map>

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                       Class adiosCore Declaration
\*---------------------------------------------------------------------------*/
class adiosCore
{

        //- ADIOS instance
        static std::unique_ptr<adios2::ADIOS> adiosPtr_;

        static bool instantiated;

        // Map to adios2::IO definitions - mirror of InquireIO(name)
        static std::map<word, std::unique_ptr<adios2::IO> > ioMap;

        // Map to adios2::Engine definitions - inquiring Engine
        static std::map<word, std::shared_ptr<adios2::Engine> > engineMap;

        //- ADIOS output IO definitions - mirror of InquireIO("write")
        std::unique_ptr<adios2::IO> ioPtr_;

        //- ADIOS file write - mirror of ...
        std::shared_ptr<adios2::Engine> enginePtr_;

        std::unique_ptr<adios2::Variable<char> > variablePtr_;

public:

    //- Constructor
    adiosCore();

    //- Copy constructor
    adiosCore(const adiosCore&);

    //- Destructor
    ~adiosCore();

    //- Copy assignment
    adiosCore& operator=(const adiosCore&);

    bool defineVariable
    (
        const word name,
        const label shape,
        const label start,
        const label count,
        const bool constantDims = false
    );

    bool open(const fileName pathname);

    void beginStep();

    bool put(const char* buf);

    void endStep();

    void close();

};

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
