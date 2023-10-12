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

#include "Offsets.H"

#include "globalIndex.H"


Foam::Offsets::Offsets(label value, bool reduce)
{
    set(value, reduce);
}


void Foam::Offsets::set(Foam::label value, bool reduce)
{
    Foam::globalIndex offsets(value, reduce);

    // Guarantee monotonically increasing values
    offset_pairs_[0].second = offsets.offset(1);
    for (label proc = 1; proc < Foam::Pstream::nProcs(); ++proc)
    {
        offset_pairs_[proc].second = offsets.offset(proc+1);
        if (offset_pairs_[proc].second < offset_pairs_[proc-1].second)
        {
            offset_pairs_[proc].second = offset_pairs_[proc-1].second;
        }
    }

    // Translate offsets through lower bound -- first in pair
    for (auto it = offset_pairs_.begin(); it < offset_pairs_.end()-1; ++it)
    {
        (it+1)->first = it->second;
    }
}


Foam::label Foam::Offsets::lowerBound(Foam::label myProcNo) const
{
    return offset_pairs_[myProcNo].first;
}


Foam::label Foam::Offsets::upperBound(Foam::label myProcNo) const
{
    return offset_pairs_[myProcNo].second;
}


Foam::label Foam::Offsets::count(Foam::label myProcNo) const
{
    return upperBound(myProcNo) - lowerBound(myProcNo);
}


Foam::label Foam::Offsets::size() const
{
    return count(Pstream::myProcNo());
}


Foam::Offsets::iterator Foam::Offsets::begin()
{
    return offset_pairs_.begin();
}


Foam::Offsets::iterator Foam::Offsets::end()
{
    return offset_pairs_.end();
}


std::pair<Foam::label, Foam::label>
Foam::Offsets::operator[](label i) const
{
    return offset_pairs_[i];
}

