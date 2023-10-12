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
    Foam::Slice

Description
    Defining a mesh slice of a cell/face/point list.

Author
    Gregor Weiss, HLRS University of Stuttgart, 2023
    Sergey Lesnik, Wikki GmbH, 2023
    Henrik Rusche, Wikki GmbH, 2023

\*---------------------------------------------------------------------------*/

// * * * * * * * * * * * * * * Free Functions  * * * * * * * * * * * * * * //

template<typename Container>
Foam::label Foam::numCellsFromSlice
(
    const Container& neighbours,
    const Foam::Slice& slice
)
{
    return std::count_if
           (
               std::begin(neighbours),
               std::end(neighbours),
               slice
           );
}


template<typename Container, typename Operator>
void Foam::appendTransformed
(
    Container& ret,
    Container& input,
    Operator operation
)
{
    std::transform
    (
        std::begin(input),
        std::end(input),
        std::begin(input),
        operation
    );
    ret.append(input);
}

// * * * * * * * * * * * * Public Member Functions * * * * * * * * * * * * //

template<typename Container>
typename std::enable_if<Foam::is_range<Container>::value, Container>::type
Foam::Slice::convert(Container& list) const
{
    typedef typename Container::value_type index_type;
    std::transform
    (
        std::begin(list),
        std::end(list),
        std::begin(list),
        [this](const index_type& id)
        {
            return convert(id);
        }
    );
    return list;
}


template<typename Container>
void Foam::Slice::append(const Container& list)
{
    mapping_->append(list);
}

// ************************************************************************* //
