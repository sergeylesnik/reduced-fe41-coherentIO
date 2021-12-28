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

#include "IO.h"
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


public:

    // Declare name of the class and its debug switch
    ClassName("adiosCore");


    //- Constructors

        adiosCore();

    //- Disallow copy constructor
    adiosCore(const adiosCore&) = delete;

    //- Destructor
    ~adiosCore();

    //- Disallow copy assignment
    adiosCore& operator=(const adiosCore&) = delete;

    // Access

        //- ADIOS instance
        std::unique_ptr<adios2::ADIOS>& adiosPtr();
};

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
