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

Class
    Foam::ProcessorPatch

Description
    Required for processor patch construction on coherent mesh layout.

Author
    Gregor Weiss, HLRS University of Stuttgart, 2023
    Sergey Lesnik, Wikki GmbH, 2023
    Henrik Rusche, Wikki GmbH, 2023

\*---------------------------------------------------------------------------*/

// * * * * * * * * * * * * * * Free Functions  * * * * * * * * * * * * * * //

template<typename SomeList, typename IndexList>
SomeList Foam::extractor(const SomeList& input, const IndexList& extractorList)
{
    typedef typename IndexList::value_type index_type;
    SomeList output;
    output.resize(extractorList.size());
    std::transform
    (
        std::begin(extractorList),
        std::end(extractorList),
        std::begin(output),
        [&input](const index_type& id)
        {
            return input[id];
        }
    );
    return output;
}

// * * * * * * * * * * * * Public Member Functions * * * * * * * * * * * * //

template<typename FaceList>
FaceList Foam::ProcessorPatch::extractFaces(const FaceList& input)
{
    return Foam::extractor(input, localFaceIDs_);
}

// ************************************************************************* //
