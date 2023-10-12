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
    Foam::SliceBuffer

Description
    Reading and writing the data for parallel I/O into buffers for
    deferred I/O.

Author
    Gregor Weiss, HLRS University of Stuttgart, 2023
    Sergey Lesnik, Wikki GmbH, 2023
    Henrik Rusche, Wikki GmbH, 2023

SourceFiles
    SliceBuffer.C

\*---------------------------------------------------------------------------*/

#ifndef SliceBuffer_H
#define SliceBuffer_H

#include "adios2.h"

#include <memory>

#include "label.H"
#include "labelList.H"
#include "vector.H"
#include "messageStream.H" // FatalError()

namespace Foam
{

// Convert Foam::labelList to adios2::Dims
adios2::Dims toDims(const Foam::labelList& list);

class SliceBuffer
{
    virtual void v_transfer
    (
        adios2::Engine*,
        const char*,
        const labelList& = {},
        const bool = false
    );

    virtual void v_transfer
    (
        adios2::Engine*,
        char*,
        const labelList& = {},
        const bool = false
    ) {};


    virtual void v_transfer
    (
        adios2::Engine*,
        const label*,
        const labelList& = {},
        const bool = false
    ) {};


    virtual void v_transfer
    (
        adios2::Engine*,
        label*,
        const labelList& = {},
        const bool = false
    ) {};


    virtual void v_transfer
    (
        adios2::Engine*,
        const scalar*,
        const labelList& = {},
        const bool = false
    ) {};


    virtual void v_transfer
    (
        adios2::Engine*,
        scalar*,
        const labelList& = {},
        const bool = false
    ) {};

public:

    SliceBuffer() = default;

    virtual ~SliceBuffer() = default;

    virtual label size() = 0;


    // Note: constness is included in with type deduction of DataType
    template<typename DataType>
    void transfer
    (
        adios2::Engine* engine,
        DataType* const data,
        const labelList& mapping = {},
        const bool masked = false
    )
    {
        if (!engine)
        {
            FatalErrorInFunction
                << "Engine is nullptr" << abort(FatalError);
        }
        if (!data && size() > 0)
        {
            FatalErrorInFunction
                << "Data is nullptr" << abort(FatalError);
        }
        v_transfer(engine, data, mapping, masked);
    }
};

}

#endif
