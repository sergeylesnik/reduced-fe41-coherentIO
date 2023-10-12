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
    Foam::FileSliceStream

Description
    Child class from SliceStream implementing the parallel I/O streaming
    into a file

Author
    Gregor Weiss, HLRS University of Stuttgart, 2023
    Sergey Lesnik, Wikki GmbH, 2023
    Henrik Rusche, Wikki GmbH, 2023

SourceFiles
    FileSliceStream.C

\*---------------------------------------------------------------------------*/

#ifndef FileSliceStream_H
#define FileSliceStream_H

#include "SliceStream.H"

namespace Foam
{

// Forward declarations
class string;
class StreamFeatures;

class FileSliceStream
:
    public SliceStream
{

    std::unique_ptr<StreamFeatures> sliceFile_;

    virtual void v_access() final;

    virtual void v_flush() final;

public:

    explicit FileSliceStream(std::unique_ptr<StreamFeatures>&);

};

}

#endif
