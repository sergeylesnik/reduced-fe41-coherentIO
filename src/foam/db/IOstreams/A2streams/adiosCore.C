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

#include "Pstream.H"

#include "adiosCore.H"

// * * * * * * * * * * * * * * * Static Members  * * * * * * * * * * * * * * //

std::unique_ptr<adios2::ADIOS> Foam::adiosCore::adiosPtr_ = nullptr;
bool Foam::adiosCore::instantiated = false;


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::adiosCore::adiosCore()
:
    ioPtr_(nullptr),
    enginePtr_(nullptr)
{
    if (!instantiated)
    {
        if (Pstream::parRun())
        {
            adiosPtr_.reset(new adios2::ADIOS(Pstream::worldComm));
        }
        else
        {
            adiosPtr_.reset(new adios2::ADIOS());
        }
        instantiated = static_cast<bool>(*adiosPtr_);
    }

    try
    {
        (void) adiosPtr_->DeclareIO("read");
        (void) adiosPtr_->DeclareIO("write");
    }
    catch (...)
    {
        // Need catch when in debug mode?
    }

    ioPtr_.reset(new adios2::IO(adiosPtr_->AtIO("write")));

    ioMap.emplace
    (
        "read", 
        std::unique_ptr<adios2::IO>(new adios2::IO(adiosPtr_->AtIO("read") ) )
    );
    ioMap.emplace
    (
        "write", 
        std::unique_ptr<adios2::IO>(new adios2::IO(adiosPtr_->AtIO("write") ) )
    );
}

// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

Foam::adiosCore::~adiosCore(){}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

bool Foam::adiosCore::defineVariable
(
    const Foam::word name,
    const Foam::label shape,
    const Foam::label start,
    const Foam::label count,
    const bool constantDims
)
{
    variablePtr_.reset
    (
        new adios2::Variable<char>
        (
            ioPtr_->DefineVariable<char>
            (
                name,
                {static_cast<size_t>(shape)},
                {static_cast<size_t>(start)},
                {static_cast<size_t>(count)},
                constantDims
            )
        )
    );
    return true;
}


bool Foam::adiosCore::open(const fileName pathname)
{
    enginePtr_.reset(new adios2::Engine(ioPtr_->Open(pathname, adios2::Mode::Write)));

    return true;
}


void Foam::adiosCore::beginStep()
{
    enginePtr_->BeginStep();
}


bool Foam::adiosCore::put(const char* buf)
{
    enginePtr_->Put(*variablePtr_, buf);

    return true;
}


void Foam::adiosCore::endStep()
{
    enginePtr_->EndStep();
}


void Foam::adiosCore::close()
{
    enginePtr_->Close();
}


// ************************************************************************* //
