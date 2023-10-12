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

Author
    Gregor Weiss, HLRS University of Stuttgart, 2023
    Sergey Lesnik, Wikki GmbH, 2023
    Henrik Rusche, Wikki GmbH, 2023

\*---------------------------------------------------------------------------*/

#include "OutputFeatures.H"

#include "adios2.h"
#include "IO.h"
#include "Engine.h"

#include "SliceStreamRepo.H"

#include "fileName.H"


std::shared_ptr<adios2::IO>
Foam::OutputFeatures::createIO(adios2::ADIOS* const corePtr)
{
    SliceStreamRepo* repo = Foam::SliceStreamRepo::instance();
    std::shared_ptr<adios2::IO> ioPtr{nullptr};
    repo->pull(ioPtr, "write");
    if (!ioPtr)
    {
        ioPtr = std::make_shared<adios2::IO>(corePtr->DeclareIO("write"));
        ioPtr->SetEngine("BP5");
        repo->push(ioPtr, "write");
    }
    return ioPtr;
}


std::shared_ptr<adios2::Engine>
Foam::OutputFeatures::createEngine
(
    adios2::IO* const ioPtr,
    const Foam::fileName& path
)
{
    SliceStreamRepo* repo = Foam::SliceStreamRepo::instance();
    std::shared_ptr<adios2::Engine> enginePtr{nullptr};
    auto size = path.length();
    repo->pull(enginePtr, "write" + path(size));
    if (!enginePtr)
    {
        enginePtr = std::make_shared<adios2::Engine>
                    (
                        ioPtr->Open(path, adios2::Mode::Append)
                    );
        enginePtr->BeginStep();
        repo->push(enginePtr, "write" + path(size));
    }
    return enginePtr;
}

