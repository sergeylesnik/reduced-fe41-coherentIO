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
    Foam::SliceFeatures

Description
    Abstract factory for creation of streaming resources for parallel I/O

Author
    Gregor Weiss, HLRS University of Stuttgart, 2023
    Sergey Lesnik, Wikki GmbH, 2023
    Henrik Rusche, Wikki GmbH, 2023

\*---------------------------------------------------------------------------*/

#ifndef StreamFeatures_H
#define StreamFeatures_H

#include <memory>

namespace adios2
{
class ADIOS;
class IO;
class Engine;
}

namespace Foam
{

// Forward declaration
class fileName;

struct StreamFeatures
{
    virtual ~StreamFeatures() = default;

    virtual std::shared_ptr<adios2::IO>
    createIO(adios2::ADIOS* const) = 0;

    virtual std::shared_ptr<adios2::Engine>
    createEngine(adios2::IO* const, const fileName&) = 0;
};

}

#endif
