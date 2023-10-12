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

#include "Slice.H"

#include "Offsets.H"

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::Slice::Slice
(
    const Foam::label& partition,
    const Foam::Offsets& offsets
)
:
    Slice
    (
        partition,
        offsets.lowerBound(partition),
        offsets.upperBound(partition)
    )
{}


Foam::Slice::Slice
(
    const Foam::label& bottom,
    const Foam::label& top
)
:
    Slice(-1, bottom, top)
{}


Foam::Slice::Slice
(
    const Foam::label& partition,
    const Foam::label& bottom,
    const Foam::label& top
)
:
    partition_{partition},
    bottom_{bottom},
    top_{top},
    mapping_{std::make_shared<Foam::sliceMap>(top_-bottom_)}
{}

// * * * * * * * * * * * * Private Member Functions * * * * * * * * * * * * //

Foam::label Foam::Slice::shift(const Foam::label& id) const
{
    return id - bottom_;
}

// * * * * * * * * * * * * Public Member Functions * * * * * * * * * * * * //

Foam::label Foam::Slice::partition() const
{
    return partition_;
}


bool Foam::Slice::operator()(const Foam::label& id) const
{
    return (bottom_<=id && id<top_);
}


bool Foam::Slice::exist(const Foam::label& id) const
{
    return (mapping_->exist(id)) || (this->operator()(id));
}


Foam::label Foam::Slice::convert(const Foam::label& id) const
{
    return this->operator()(id) ? shift(id) : mapping_->operator[](id);
}

// ************************************************************************* //
