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

Author
    Gregor Weiss, HLRS University of Stuttgart, 2023
    Sergey Lesnik, Wikki GmbH, 2023
    Henrik Rusche, Wikki GmbH, 2023

\*---------------------------------------------------------------------------*/

#include "FileSliceStream.H"
#include "StreamFeatures.H"

#include "SliceStreamImpl.H"

#include "foamString.H"


Foam::FileSliceStream::FileSliceStream
(
    std::unique_ptr<StreamFeatures>& fileFeatures
)
:
    sliceFile_{std::move(fileFeatures)}
{}


void Foam::FileSliceStream::v_access()
{
    Foam::SliceStreamRepo* repo = Foam::SliceStreamRepo::instance();
    ioPtr_ = sliceFile_->createIO(repo->pullADIOS());
    enginePtr_ = sliceFile_->createEngine(ioPtr_.get(), paths_.getPathName());
}


void Foam::FileSliceStream::v_flush()
{
    Foam::SliceStreamRepo* repo = Foam::SliceStreamRepo::instance();
    repo->close();
    v_access();
}
