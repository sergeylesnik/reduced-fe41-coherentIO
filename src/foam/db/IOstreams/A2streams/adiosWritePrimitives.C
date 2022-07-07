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

#include "adiosWritePrimitives.H"

#include "adiosWriting.H"
#include "adiosFileStream.H"

#include "vector.H"

void Foam::adiosWritePrimitives( const Foam::string type,
                                 const Foam::string blockId,
                                 const Foam::label count,
                                 const Foam::scalar* buf ) {
    auto adiosStreamPtr = adiosWriting{}.createStream();
    adiosStreamPtr->open( type );
    adiosStreamPtr->transfer( blockId, count, 0, count, buf );
}

void Foam::adiosWritePrimitives( const Foam::string type,
                                 const Foam::string blockId,
                                 const Foam::label count,
                                 const Foam::label* buf ) {
    auto adiosStreamPtr = adiosWriting{}.createStream();
    adiosStreamPtr->open( type );
    adiosStreamPtr->transfer( blockId, count, 0, count, buf );
}

void Foam::adiosWritePrimitives( const Foam::string type,
                                 const Foam::string blockId,
                                 const Foam::label count,
                                 const Foam::Vector<Foam::scalar>* buf ) {
    Foam::labelList iShape{2};
    iShape[0] = count;
    iShape[1] = buf[0].size();
    Foam::labelList iStart{2};
    iStart[0] = iStart[1] = 0;
    Foam::labelList iCount{ iShape };

    auto adiosStreamPtr = adiosWriting{}.createStream();
    adiosStreamPtr->open( type );
    adiosStreamPtr->transfer( blockId, iShape, iStart, iCount, reinterpret_cast<const Foam::scalar*>( buf ) );
}

// ************************************************************************* //
