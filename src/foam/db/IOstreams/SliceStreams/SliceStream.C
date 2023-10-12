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

#include "SliceStream.H"

#include "SliceStreamImpl.H"

// * * * * * * * * * * * * * * * * Constructor  * * * * * * * * * * * * * * //

Foam::SliceStream::SliceStream()
:
    pimpl_{new Impl()},
    paths_{},
    type_{},
    ioPtr_{nullptr},
    enginePtr_{nullptr}
{}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * //

Foam::SliceStream::~SliceStream() = default;


std::string Foam::SliceStreamType(const std::string& id)
{
    std::string type = "fields";
    if (id.find("polyMesh") != std::string::npos
        ||
        id.find("region") != std::string::npos)
    {
        type = "mesh";
    }
    return type;
}

// * * * * * * * * * * * * * Private Member Functions * * * * * * * * * * * //

void
Foam::SliceStream::setPath(const Foam::string& type, const Foam::string& path)
{
    if (type == "mesh")
    {
        paths_.setPathName(paths_.meshPathname(path));
    }
    else if (type == "fields")
    {
        paths_.setPathName(paths_.dataPathname(path));
    }
}

// * * * * * * * * * * * * * Public Member Functions * * * * * * * * * * * //

void Foam::SliceStream::access(const Foam::string& type, const Foam::string& path)
{
    type_ = type;
    setPath(type, path);
    v_access();
}


void Foam::SliceStream::bufferSync()
{
    if (enginePtr_)
    {
        if
        (
            enginePtr_->OpenMode() == adios2::Mode::Read
         || enginePtr_->OpenMode() == adios2::Mode::ReadRandomAccess
        )
        {
            enginePtr_->PerformGets();
        }
        else
        {
            enginePtr_->PerformPuts();
        }
    }
}


Foam::label Foam::SliceStream::getBufferSize
(
    const Foam::string& blockId,
    const Foam::scalar* const data
)
{
    return pimpl_->readingBuffer<Foam::variableBuffer<Foam::scalar> >
                   (
                       ioPtr_.get(),
                       enginePtr_.get(),
                       blockId
                   );
}


Foam::label Foam::SliceStream::getBufferSize
(
    const Foam::string& blockId,
    const Foam::label* const data
)
{
    return pimpl_->readingBuffer<Foam::variableBuffer<Foam::label> >
                   (
                       ioPtr_.get(),
                       enginePtr_.get(),
                       blockId
                   );
}


Foam::label Foam::SliceStream::getBufferSize
(
    const Foam::string& blockId,
    const char* const data
)
{
    return pimpl_->readingBuffer<Foam::variableBuffer<char> >
                   (
                       ioPtr_.get(),
                       enginePtr_.get(),
                       blockId
                   );
}


// Reading
void Foam::SliceStream::get
(
    const Foam::string& blockId,
    scalar* data,
    const Foam::labelList& start,
    const Foam::labelList& count
)
{
    pimpl_->get
            (
                ioPtr_.get(),
                enginePtr_.get(),
                blockId,
                data,
                start,
                count
            );
}


void Foam::SliceStream::get
(
    const Foam::string& blockId,
    label* data,
    const Foam::labelList& start,
    const Foam::labelList& count
)
{
    pimpl_->get
            (
                ioPtr_.get(),
                enginePtr_.get(),
                blockId,
                data,
                start,
                count
            );
}


void Foam::SliceStream::get
(
    const Foam::string& blockId,
    char* data,
    const Foam::labelList& start,
    const Foam::labelList& count
)
{
    pimpl_->get
            (
                ioPtr_.get(),
                enginePtr_.get(),
                blockId,
                data,
                start,
                count
            );
}


// Writing
void Foam::SliceStream::put
(
    const Foam::string& blockId,
    const Foam::labelList& shape,
    const Foam::labelList& start,
    const Foam::labelList& count,
    const scalar* data,
    const Foam::labelList& mapping,
    const bool masked
)
{
    pimpl_->put
            (
                ioPtr_.get(),
                enginePtr_.get(),
                blockId,
                shape,
                start,
                count,
                data,
                mapping,
                masked
            );
}


void Foam::SliceStream::put
(
    const Foam::string& blockId,
    const Foam::labelList& shape,
    const Foam::labelList& start,
    const Foam::labelList& count,
    const label* data,
    const Foam::labelList& mapping,
    const bool masked
)
{
    pimpl_->put
            (
                ioPtr_.get(),
                enginePtr_.get(),
                blockId,
                shape,
                start,
                count,
                data,
                mapping,
                masked
            );
}


void Foam::SliceStream::put
(
    const Foam::string& blockId,
    const Foam::labelList& shape,
    const Foam::labelList& start,
    const Foam::labelList& count,
    const char* data,
    const Foam::labelList& mapping,
    const bool masked
)
{
    pimpl_->put
            (
                ioPtr_.get(),
                enginePtr_.get(),
                blockId,
                shape,
                start,
                count,
                data,
                mapping,
                masked
            );
}


void Foam::SliceStream::flush()
{
    v_flush();
}

// ************************************************************************* //
