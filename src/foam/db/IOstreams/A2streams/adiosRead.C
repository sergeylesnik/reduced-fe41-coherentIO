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

#include "adiosRead.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

std::unique_ptr<adios2::IO> Foam::adiosRead::ioReadPtr_ = nullptr;

std::unique_ptr<adios2::IO> Foam::adiosRead::ioReadMeshPtr_ = nullptr;

std::unique_ptr<adios2::Engine> Foam::adiosRead::enginePtr_ = nullptr;

std::unique_ptr<adios2::Engine> Foam::adiosRead::engineMeshPtr_ = nullptr;


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::adiosRead::adiosRead()
:
    pathname_(adiosCore::dataPathname()),
    variablePtr_(nullptr)
{}


// * * * * * * * * * * * * * * * * Destructors * * * * * * * * * * * * * * * //

Foam::adiosRead::~adiosRead()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

Foam::fileName Foam::adiosRead::pathname()
{
    return this->pathname_;
}


std::unique_ptr<adios2::IO>& Foam::adiosRead::ioReadPtr()
{
    if(!ioReadPtr_)
    {
        ioReadPtr_.reset
        (
            new adios2::IO(adiosPtr()->DeclareIO("read"))
        );
    }

    return ioReadPtr_;
}


std::unique_ptr<adios2::IO>& Foam::adiosRead::ioReadMeshPtr()
{
    if(!ioReadMeshPtr_)
    {
        ioReadMeshPtr_.reset
        (
            new adios2::IO(adiosPtr()->DeclareIO("readMesh"))
        );
    }

    return ioReadMeshPtr_;
}


std::unique_ptr<adios2::Engine>& Foam::adiosRead::enginePtr()
{
    if(!enginePtr_)
    {
        enginePtr_.reset
        (
            new adios2::Engine
            (
                ioReadPtr()->Open(pathname_, adios2::Mode::Read)
            )
        );
    }

    return enginePtr_;
}


std::unique_ptr<adios2::Engine>& Foam::adiosRead::engineMeshPtr()
{
    if(!engineMeshPtr_)
    {
        engineMeshPtr_.reset
        (
            new adios2::Engine
            (
                ioReadMeshPtr()->Open
                (
                    adiosCore::meshPathname(),
                    adios2::Mode::Read
                )
            )
        );
    }

    return engineMeshPtr_;
}


void Foam::adiosRead::open()
{
    enginePtr();
}


void Foam::adiosRead::defineVariable(const string name)
{
    variablePtr_.reset
    (
        new adios2::Variable<double>
        (
            ioReadPtr()->InquireVariable<double>(name)
        )
    );
}


void Foam::adiosRead::get(double* buf)
{
    enginePtr()->Get<double>(*variablePtr_, buf);
}


void Foam::adiosRead::performGets()
{
    if (adiosCore::debug)
    {
        Pout<< "adiosRead::performGets()" << endl;
    }

    enginePtr()->PerformGets();
}


void Foam::adiosRead::read
(
    parIOType* buf,
    const Foam::string blockId
)
{
    adios2::Variable<parIOType> var;

    // Use only the pointer infrastructure from this class by now for clarity
    if (dataPresent())
    {
        open();
        var = ioReadPtr()->InquireVariable<parIOType>(blockId);
    }

    if (var)
    {
        enginePtr()->Get<parIOType>(var, buf, adios2::Mode::Sync);
    }
    else if (meshPresent())
    {
        engineMeshPtr();
        var = ioReadMeshPtr()->InquireVariable<parIOType>(blockId);
        engineMeshPtr()->Get(var, buf, adios2::Mode::Sync);
    }

    // open();
    // defineVariable(blockId);
    // get(buf);
    // performGets();
}


bool Foam::adiosRead::readLocalString
(
    std::string& buf,
    const Foam::string strName
)
{
    if (adiosCore::debug)
    {
        Pout<< "Read local string: " << strName << endl;
    }

    // Use only the pointer infrastructure from this class by now for clarity
    adios2::Variable<char> var;
    if (dataPresent())
    {
        enginePtr();
        var = ioReadPtr()->InquireVariable<char>(strName);
    }

    bool found = false;
    if (var)
    {
        // Resize the buffer according to the size of the variable.
        // SetSelection() not needed since the variable is local to each proc.
        buf.resize(var.SelectionSize());
        enginePtr()->Get(var, &buf[0], adios2::Mode::Sync);
        found = true;
    }
    else if (meshPresent())
    {
        // Try to look up in the mesh file
        engineMeshPtr();
        var = ioReadMeshPtr()->InquireVariable<char>(strName);

        if (var)
        {
            buf.resize(var.SelectionSize());
            engineMeshPtr()->Get<char>(var, &buf[0], adios2::Mode::Sync);
            found = true;
        }
    }

    return found;
}

// ************************************************************************* //
