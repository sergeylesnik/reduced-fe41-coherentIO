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

#ifndef adiosWritePrimitives_H
#define adiosWritePrimitives_H

#include "label.H"
#include "scalar.H"

#include "primitives_traits.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

class string;
template<typename T> class List;

List<label> create2DList( const label& val0, const label& val1 );

void adiosWritePrimitives( const string type,
                           const string blockId,
                           const label count,
                           const scalar* buf );

void adiosWritePrimitives( const string type,
                           const string blockId,
                           const label count,
                           const label* buf );

void adiosWritePrimitives( const string type,
                           const string blockId,
                           const List<label> shape,
                           const List<label> start,
                           const List<label> count,
                           const scalar* buf );

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

template< typename T, std::enable_if_t< !Foam::is_vectorspace<T>::value, void >* = nullptr >
void Foam::adiosWritePrimitives( const Foam::string type,
                                 const Foam::string name,
                                 const Foam::label count,
                                 const T* buf ) {
    std::cout << "adiosWritePrimitives not implemented for this type.\n";
    std::cout << typeid( buf ).name() << "\n";
    std::cout << name << "\n";
}

template< typename T, std::enable_if_t< Foam::is_vectorspace<T>::value, void >* = nullptr >
void Foam::adiosWritePrimitives( const Foam::string type,
                                 const Foam::string name,
                                 const Foam::label count,
                                 const T* buf ) {
    Foam::List<Foam::label> shapeList = create2DList( count, buf[0].size() );
    Foam::List<Foam::label> startList = create2DList( 0, 0 );
    Foam::List<Foam::label> countList{ shapeList };
    adiosWritePrimitives( type,
                          name,
                          shapeList,
                          startList,
                          countList,
                          reinterpret_cast<const Foam::scalar*>( buf ) );
}

#endif

// ************************************************************************* //
