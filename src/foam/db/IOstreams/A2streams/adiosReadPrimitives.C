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

#include "adiosReadPrimitives.H"
#include "adiosReading.H"
#include "adiosFileStream.H"

#include "vector.H"
//#include "List.H"

void Foam::adiosReadPrimitives( const Foam::string type,
                                const Foam::string blockId,
                                Foam::scalar* buf ) {
    auto adiosStreamPtr = adiosReading{}.createStream();
    adiosStreamPtr->open( type );
    adiosStreamPtr->transfer( blockId, buf );
}

void Foam::adiosReadPrimitives( const Foam::string type,
                                const Foam::string blockId,
                                Foam::scalar* buf,
                                const label& start,
                                const label& count ) {
    labelList startList( 1 ); startList[0] = start;
    labelList countList( 1 ); countList[0] = count;
    auto adiosStreamPtr = adiosReading{}.createStream();
    adiosStreamPtr->open( type );
    adiosStreamPtr->transfer( blockId, buf, startList, countList );
}

void Foam::adiosReadPrimitives( const Foam::string type,
                                const Foam::string blockId,
                                Foam::label* buf ) {
    auto adiosStreamPtr = adiosReading{}.createStream();
    adiosStreamPtr->open( type );
    adiosStreamPtr->transfer( blockId, buf );
}

void Foam::adiosReadPrimitives( const Foam::string type,
                                const Foam::string blockId,
                                Foam::label* buf,
                                const label& start,
                                const label& count ) {
    labelList startList( 1 ); startList[0] = start;
    labelList countList( 1 ); countList[0] = count;
    auto adiosStreamPtr = adiosReading{}.createStream();
    adiosStreamPtr->open( type );
    adiosStreamPtr->transfer( blockId, buf, startList, countList );
}

void Foam::adiosReadPrimitives( const Foam::string type,
                                const Foam::string blockId,
                                char* buf ) {
    auto adiosStreamPtr = adiosReading{}.createStream();
    adiosStreamPtr->open( type );
    adiosStreamPtr->transfer( blockId, buf );
}

void Foam::adiosReadPrimitives( const Foam::string type,
                                const Foam::string blockId,
                                char* buf,
                                const label& start,
                                const label& count ) {
    labelList startList( 1 ); startList[0] = start;
    labelList countList( 1 ); countList[0] = count;
    auto adiosStreamPtr = adiosReading{}.createStream();
    adiosStreamPtr->open( type );
    adiosStreamPtr->transfer( blockId, buf, startList, countList );
}

void Foam::adiosReadPrimitives( const Foam::string type,
                                const Foam::string blockId,
                                Foam::Vector<Foam::scalar>* buf ) {
    auto adiosStreamPtr = adiosReading{}.createStream();
    adiosStreamPtr->open( type );
    adiosStreamPtr->transfer( blockId, reinterpret_cast<Foam::scalar*>( buf ) );
}

void Foam::adiosReadPrimitives( const Foam::string type,
                                const Foam::string blockId,
                                Foam::Vector<Foam::scalar>* buf,
                                const Foam::List<Foam::label>& start,
                                const Foam::List<Foam::label>& count ) {
    auto adiosStreamPtr = adiosReading{}.createStream();
    adiosStreamPtr->open( type );
    adiosStreamPtr->transfer( blockId, reinterpret_cast<Foam::scalar*>( buf ), start, count );
}

// ************************************************************************* //
