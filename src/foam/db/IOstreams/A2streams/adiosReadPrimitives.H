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

#include "List.H"
#include "label.H"
#include "scalar.H"

#include "primitives_traits.H"

#include <initializer_list>

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

class string;

List<Foam::label> createList( std::initializer_list<Foam::label> input );

void adiosReadPrimitives( const string type,
                          const string name,
                          scalar* buf,
                          const List<label>& start = {},
                          const List<label>& count = {} );

void adiosReadPrimitives( const string type,
                          const string name,
                          label* buf,
                          const List<label>& start = {},
                          const List<label>& count = {} );

void adiosReadPrimitives( const string type,
                          const string name,
                          char* buf,
                          const List<label>& start = {},
                          const List<label>& count = {} );

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

template< typename T, std::enable_if_t< !Foam::is_vectorspace<T>::value, void >* = nullptr >
void Foam::adiosReadPrimitives( const Foam::string type, 
                               const Foam::string name,
                               T* buf, 
                               const Foam::List<Foam::label>& start = {},
                               const Foam::List<Foam::label>& count = {} ) {
    std::cout << "adiosReadPrimitives not implemented for this type.\n";
    std::cout << typeid( buf ).name() << "\n";
    std::cout << name << "\n";
}


template< typename T, std::enable_if_t< Foam::is_vectorspace<T>::value, void >* = nullptr >
void Foam::adiosReadPrimitives( const Foam::string type,
                                const Foam::string name,
                                T* buf,
                                const Foam::List<Foam::label>& start = {},
                                const Foam::List<Foam::label>& count = {} ) {
    auto startList = start;
    auto countList = count;
    if ( start.size() > 0 && count.size() > 0 ) {
        startList = Foam::createList( { start[0], 0 } );
        countList = Foam::createList( { count[0], buf[0].size() } );
    }
    Foam::adiosReadPrimitives( type, name, reinterpret_cast<scalar*>( buf ), startList, countList );
}


template< typename T,
          typename Integral,
          std::enable_if_t< std::is_arithmetic<T>::value, void >* = nullptr,
          std::enable_if_t< std::is_integral<Integral>::value, void >* = nullptr >
void Foam::adiosReadPrimitives( const Foam::string type,
                                const Foam::string name,
                                T* buf,
                                Integral start = -1,
                                Integral count = -1 ) {
    Foam::List<Integral> startList{};
    Foam::List<Integral> countList{};
    if ( start > -1 && count > -1 ) {
        startList = Foam::createList( { start } );
        countList = Foam::createList( { count } );
    }
    Foam::adiosReadPrimitives( type, name, buf, startList, countList );
}


#endif

// ************************************************************************* //
