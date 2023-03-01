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
    Foam::fieldTag

Description
    Tag class corresponds to a field and contains properties needed for the
    decision on the global uniformity of the field

SourceFiles
    fieldTag.C

\*---------------------------------------------------------------------------*/

#ifndef fieldTag_H
#define fieldTag_H

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include "scalarList.H"

namespace Foam
{

class fieldTag
{

public:

    // Public fieldTag types

        enum uniformity
        :
            label
        {
            EMPTY,
            UNIFORM,
            NONUNIFORM
        };


private:

    // Private data

        //- Uniformity property of the field
        uniformity uniformity_;

        //- First element of the field
        scalarList firstElement_;


public:

    // Constructors

        fieldTag();


    // Member functions

        // Static Member Functions

            //- Binary operator to be used by reduce function for detecting
            //  global uniformity
            static List<fieldTag>
            globalUniform(const List<fieldTag>& x, const List<fieldTag>& y);

        // Access

            inline const scalarList& firstElement() const
            {
                return firstElement_;
            }

            inline scalarList& firstElement()
            {
                return firstElement_;
            }

            inline uniformity getUniformity() const
            {
                return uniformity_;
            }

            inline uniformity& getUniformity()
            {
                return uniformity_;
            }

        // Required by List class

            bool operator==(const fieldTag&) const;
            bool operator!=(const fieldTag&) const;
};


// Required by Pstream

Ostream& operator<<
(
    Ostream& os,
    const fieldTag& d
);

Istream& operator>>
(
    Istream& is,
    fieldTag& d
);

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
