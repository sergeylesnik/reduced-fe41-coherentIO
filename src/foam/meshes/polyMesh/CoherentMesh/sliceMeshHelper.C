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

#include "sliceMeshHelper.H"

// * * * * * * * * * * * * * * Free Functions * * * * * * * * * * * * * * * //

Foam::label Foam::encodeSlicePatchId(const Foam::label& Id)
{
    return -(Id + 1);
}


Foam::label Foam::decodeSlicePatchId(const Foam::label& encodedId)
{
    return -(encodedId + 1);
}


void Foam::partitionByFirst(Foam::pairVector<Foam::label, Foam::label>& input)
{
    std::stable_partition
    (
        input.begin(),
        input.end(),
        [](const std::pair<Foam::label, Foam::label>& n)
        {
            return n.first>0;
        }
    );
    std::stable_sort
    (
        std::partition_point
        (
            input.begin(),
            input.end(),
            [](const std::pair<Foam::label, Foam::label>& n)
            {
                return n.first>0;
            }
        ),
        input.end(),
        []
        (
            const std::pair<Foam::label, Foam::label>& n,
            const std::pair<Foam::label, Foam::label>& m
        )
        {
            return n.first>m.first;
        }
    );
}


void
Foam::renumberFaces(Foam::faceList& faces, const std::vector<Foam::label>& map)
{
    std::for_each
    (
        faces.begin(),
        faces.end(),
        [&map](Foam::face& input)
        {
            std::transform
            (
                input.begin(),
                input.end(),
                input.begin(),
                [&map](const Foam::label& id)
                {
                    return map[id];
                }
            );
        }
    );
}


std::vector<Foam::label>
Foam::permutationOfSorted(const Foam::labelList& input)
{
    std::vector<Foam::label> indices{};
    indexIota(indices, input.size(), 0);
    indexSort(indices, input);
    return indices;
}

// ************************************************************************* //
