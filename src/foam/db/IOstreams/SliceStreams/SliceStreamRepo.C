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

#include "SliceStreamRepo.H"

#include "adios2.h"

#include "Pstream.H"
#include "foamString.H"

Foam::SliceStreamRepo* Foam::SliceStreamRepo::repoInstance_ = nullptr;

struct Foam::SliceStreamRepo::Impl
{

    using ADIOS_uPtr = std::unique_ptr<adios2::ADIOS>;
    using IO_map_uPtr = std::unique_ptr<Foam::SliceStreamRepo::IO_map>;
    using Engine_map_uPtr = std::unique_ptr<Foam::SliceStreamRepo::Engine_map>;


    // Default constructor
    Impl()
    :
        adiosPtr_{nullptr},
        ioMap_{new Foam::SliceStreamRepo::IO_map()},
        engineMap_{new Foam::SliceStreamRepo::Engine_map()}
    {
        if (!adiosPtr_)
        {
            if (Pstream::parRun())
            {
                adiosPtr_.reset
                          (
                              new adios2::ADIOS
                                  (
                                      "system/config.xml",
                                      MPI_COMM_WORLD
                                  )
                          );
            }
            else
            {
                adiosPtr_.reset(new adios2::ADIOS("system/config.xml"));
            }
        }
    }


    ADIOS_uPtr adiosPtr_{};

    IO_map_uPtr ioMap_{};

    Engine_map_uPtr engineMap_{};
};


Foam::SliceStreamRepo::SliceStreamRepo()
:
    pimpl_{new Foam::SliceStreamRepo::Impl{}},
    boundaryCounter_{0}
{}


Foam::SliceStreamRepo::~SliceStreamRepo() = default;


Foam::SliceStreamRepo* Foam::SliceStreamRepo::instance()
{
    if (!repoInstance_)
    {
        repoInstance_ = new Foam::SliceStreamRepo();
    }
    return repoInstance_;
}


adios2::ADIOS*
Foam::SliceStreamRepo::pullADIOS()
{
    return pimpl_->adiosPtr_.get();
}


Foam::SliceStreamRepo::IO_map*
Foam::SliceStreamRepo::get(const std::shared_ptr<adios2::IO>&)
{
    return pimpl_->ioMap_.get();
}


Foam::SliceStreamRepo::Engine_map*
Foam::SliceStreamRepo::get(const std::shared_ptr<adios2::Engine>&)
{
    return pimpl_->engineMap_.get();
}


void Foam::SliceStreamRepo::push(const Foam::label& input)
{
    boundaryCounter_ = input;
}


void Foam::SliceStreamRepo::open(const bool atScale)
{
    for (const auto& enginePair: *(pimpl_->engineMap_))
    {
        if (*(enginePair.second))
        {
            if (enginePair.second->OpenMode() != adios2::Mode::ReadRandomAccess)
            {
                enginePair.second->BeginStep();
            }
        }
    }
}

void Foam::SliceStreamRepo::close(const bool atScale)
{
    for (const auto& enginePair: *(pimpl_->engineMap_))
    {
        if (*(enginePair.second))
        {
            if (enginePair.second->OpenMode() != adios2::Mode::ReadRandomAccess)
            {
                enginePair.second->EndStep();
            }
            if (!atScale)
            {
                enginePair.second->Close();
            }
        }
    }

    if (!atScale)
    {
        pimpl_->engineMap_->clear();
    }
}

void Foam::SliceStreamRepo::clear()
{
    close();
    pimpl_->adiosPtr_->FlushAll();
    for (const auto& ioPair: *(pimpl_->ioMap_))
    {
        ioPair.second->RemoveAllVariables();
    }
}
