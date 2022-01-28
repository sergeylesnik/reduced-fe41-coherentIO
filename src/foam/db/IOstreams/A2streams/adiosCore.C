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
#include "fileName.H"
#include "OSspecific.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

defineTypeNameAndDebug(Foam::adiosCore, 0);

std::unique_ptr<adios2::ADIOS> Foam::adiosCore::adiosPtr_ = nullptr;

const Foam::fileName Foam::adiosCore::meshPathname_ = "constant/polyMesh.bp";

const Foam::fileName Foam::adiosCore::dataPathname_ = "data.bp";

bool Foam::adiosCore::filesChecked_ = false;

bool Foam::adiosCore::dataPresent_ = false;

bool Foam::adiosCore::meshPresent_ = false;


// * * * * * * * * * * * * Static Member Functions * * * * * * * * * * * * * //

const Foam::fileName& Foam::adiosCore::meshPathname()
{
    return meshPathname_;
}


const Foam::fileName& Foam::adiosCore::dataPathname()
{
    return dataPathname_;
}


void Foam::adiosCore::checkFiles()
{
    if (!filesChecked_)
    {
        dataPresent_ = isDir(dataPathname_);
        meshPresent_ = isDir(meshPathname_);
        filesChecked_ = true;
    }
}


bool Foam::adiosCore::dataPresent()
{
    checkFiles();
    return dataPresent_;
}


bool Foam::adiosCore::meshPresent()
{
    checkFiles();
    return meshPresent_;
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::adiosCore::adiosCore()
{
    if (debug)
    {
        Pout<< "adiosCore::adiosCore()" << endl;
    }
}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

Foam::adiosCore::~adiosCore()
{
    if (debug)
    {
        Pout<< "adiosCore::~adiosCore()" << endl;
    }
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //
std::unique_ptr<adios2::ADIOS>& Foam::adiosCore::adiosPtr()
{
    if (debug)
    {
        Pout<< "adiosCore::adiosPtr()" << endl;
    }

    if(!adiosPtr_)
    {
        if (Pstream::parRun())
        {
            if (debug)
            {
                Pout<< "Calling adios2::ADIOS(MPI_COMM_WORLD)" << endl;
            }
            adiosPtr_.reset(new adios2::ADIOS(MPI_COMM_WORLD));
        }
        else
        {
            if (debug)
            {
                Pout<< "Calling adios2::ADIOS()" << endl;
            }
            adiosPtr_.reset(new adios2::ADIOS());
        }
    }

    return adiosPtr_;
}

// ************************************************************************* //
