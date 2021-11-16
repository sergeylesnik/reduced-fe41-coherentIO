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

#include "adiosWrite.H"
#include "adiosCore.H"

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::adiosWrite::adiosWrite()
:
    pathname_(),
    adiosCorePtr_(new adiosCore())
{}

Foam::adiosWrite::adiosWrite(const fileName& pathname)
:
    pathname_(pathname),
    adiosCorePtr_(new adiosCore())
{}


// * * * * * * * * * * * * * * * * Destructors * * * * * * * * * * * * * * * //

Foam::adiosWrite::~adiosWrite() {}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

Foam::fileName Foam::adiosWrite::pathname()
{
    return this->pathname_;
}

bool Foam::adiosWrite::write
(
    const Foam::word blockId,
    const Foam::label shape,
    const Foam::label start,
    const Foam::label count,
    const char* buf
)
{
    adiosCorePtr_->defineVariable(blockId, shape, start, count, true);
    adiosCorePtr_->open(this->pathname_);
    adiosCorePtr_->beginStep();
    adiosCorePtr_->put(buf);
    adiosCorePtr_->endStep();
    adiosCorePtr_->close();

    return true;
}

// ************************************************************************* //
