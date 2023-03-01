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

#include "fieldTag.H"

// * * * * * * * * * * * * * Static Member Functions * * * * * * * * * * * * //

Foam::List<Foam::fieldTag>
Foam::fieldTag::globalUniform(const List<fieldTag>& x, const List<fieldTag>& y)
{
    List<fieldTag> res(x);

    forAll(x, i)
    {
        if
        (
            x[i].uniformity_ == NONUNIFORM
         || y[i].uniformity_ == NONUNIFORM
        )
        {
            res[i].uniformity_ = NONUNIFORM;
        }
        else if (x[i].uniformity_ == EMPTY)
        {
            res[i] = y[i];
        }
        else if
        (
            x[i].uniformity_ == UNIFORM
        )
        {
            if (y[i].uniformity_ == UNIFORM)
            {
                if (x[i].firstElement_ != y[i].firstElement_)
                {
                    res[i].uniformity_ = NONUNIFORM;
                }
            }
            else if (y[i].uniformity_ == NONUNIFORM)
            {
                res[i].uniformity_ = NONUNIFORM;
            }
        }
    }

    return res;
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::fieldTag::fieldTag()
:
    uniformity_(),
    firstElement_()
{}


// * * * * * * * * * * * * * Public Member Functions  * * * * * * * * * * * //

bool Foam::fieldTag::operator==(const fieldTag& d) const
{
    if (this->uniformity_ != d.uniformity_)
    {
        return false;
    }
    else if (this->firstElement_ != d.firstElement_)
    {
        return false;
    }

    return true;
}


bool Foam::fieldTag::operator!=(const fieldTag& d) const
{
    return !operator==(d);
}


Foam::Ostream& Foam::operator<<
(
    Ostream& os,
    const fieldTag& d
)
{
    os << d.firstElement() << d.getUniformity();
    return os;
}

Foam::Istream& Foam::operator>>
(
    Istream& is,
    fieldTag& d
)
{
    is >> d.firstElement();
    label lfu;
    is >> lfu;
    d.getUniformity() = static_cast<fieldTag::uniformity>(lfu);
    return is;
}
