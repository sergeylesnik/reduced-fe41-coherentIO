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

#ifndef adiosReadPrimitives_H
#define adiosReadPrimitives_H

#include "foamString.H"
#include "label.H"
#include "uLabel.H"
#include "scalar.H"

#include <typeinfo>

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

template<typename T> class Vector;

template<typename T>
void adiosReadPrimitives( const string, const string, T* buf );

void adiosReadPrimitives( const string, const string, scalar* );

void adiosReadPrimitives( const string, const string, label* );

void adiosReadPrimitives( const string, const string, char* );

void adiosReadPrimitives( const string, const string, Vector<scalar>* );

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

template<typename T>
void Foam::adiosReadPrimitives( const Foam::string type, const Foam::string name,
                                T* buf ) {
    std::cout << "adiosReadPrimitives not implemented for this type.\n";
    std::cout << typeid( buf ).name() << "\n";
    std::cout << name << "\n";
}


#endif

// ************************************************************************* //
