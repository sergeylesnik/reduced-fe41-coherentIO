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
    Foam::SliceStream

Description
    A parallel I/O streaming class for contiguous data access patterns

Author
    Gregor Weiss, HLRS University of Stuttgart, 2023
    Sergey Lesnik, Wikki GmbH, 2023
    Henrik Rusche, Wikki GmbH, 2023

SourceFiles
    SliceStream.C
    SliceStreamImpl.H (include in SliceStream.C)

\*---------------------------------------------------------------------------*/

#ifndef SliceStream_H
#define SliceStream_H

#include "labelList.H"

#include "SliceStreamRepo.H"

#include "SliceStreamPaths.H"

#include "SliceWriting.H"
#include "SliceReading.H"

namespace Foam
{

// Determine whether mesh or field data I/O is triggered
std::string SliceStreamType(const std::string& id);


template<typename Container>
void sliceReadToContainer
(
    const Foam::string& type,
    const Foam::string& pathname,
    const Foam::string& blockId,
    Container& container,
    const labelList& start = {},
    const labelList& count = {}
);


class SliceStream
{
    virtual void v_access() = 0;

    virtual void v_flush() = 0;

protected:

    // Forward declaration of bridge to implementation
    class Impl;

    // Bridge instance to implementation
    std::unique_ptr<Impl> pimpl_;

    // Instance on file name manager
    SliceStreamPaths paths_{};

    // Type of I/O data; field or mesh
    Foam::string type_{};

    // Pointer to io instance
    std::shared_ptr<adios2::IO> ioPtr_{nullptr};

    // Pointer to engine instance
    std::shared_ptr<adios2::Engine> enginePtr_{nullptr};

    // Setter for bp file name and path
    void setPath(const Foam::string& type, const Foam::string& path = "");

public:

    // Default constructor
    SliceStream();

    // Destructor
    virtual ~SliceStream();

    // Open engine according to mesh or field data and path
    void access(const Foam::string& type, const Foam::string& path = "");

    // Reading local/global scalar array
    void get
    (
        const string& blockId,
        scalar* data,
        const labelList& start = {},
        const labelList& count = {}
    );

    // Reading local/global label array
    void get
    (
        const string& blockId,
        label* data,
        const labelList& start = {},
        const labelList& count = {}
    );

    // Reading local/global char array
    void get
    (
        const string& blockId,
        char* data,
        const labelList& start = {},
        const labelList& count = {}
    );

    // Reading local/global array
    template<class ContainerType>
    typename std::enable_if<!std::is_const<ContainerType>::value, void>::type
    get
    (
        const string& blockId,
        ContainerType& data,
        const labelList& start = {},
        const labelList& count = {}
    );

    label getBufferSize(const Foam::string& blockId, const scalar* const data);

    label getBufferSize(const Foam::string& blockId, const label* const data);

    label getBufferSize(const Foam::string& blockId, const char* const data);

    // Writing local/global scalar array
    void put
    (
        const Foam::string& blockId,
        const Foam::labelList& shape,
        const Foam::labelList& start,
        const Foam::labelList& count,
        const scalar* buf,
        const labelList& mapping = {},
        const bool masked = false
    );

    // Writing local/global label array
    void put
    (
        const Foam::string& blockId,
        const Foam::labelList& shape,
        const Foam::labelList& start,
        const Foam::labelList& count,
        const label* buf,
        const labelList& mapping = {},
        const bool masked = false
    );

    void put
    (
        const Foam::string& blockId,
        const Foam::labelList& shape,
        const Foam::labelList& start,
        const Foam::labelList& count,
        const char* buf,
        const labelList& mapping = {},
        const bool masked = false
    );

    void bufferSync();

    void flush();

};

}


// Reading local/global array
template<class ContainerType>
typename std::enable_if<!std::is_const<ContainerType>::value, void>::type
Foam::SliceStream::get
(
    const Foam::string& blockId,
    ContainerType& container,
    const Foam::labelList& start,
    const Foam::labelList& count
)
{
    if (start.empty() && count.empty())
    {
        auto size = getBufferSize(blockId, container.data());
        container.resize(size);
    }
    else
    {
        container.resize(count[0]);
    }
    get(blockId, container.data(), start, count);
}


template<typename Container>
void Foam::sliceReadToContainer
(
    const Foam::string& type,
    const Foam::string& pathname,
    const Foam::string& blockId,
    Container& container,
    const Foam::labelList& start,
    const Foam::labelList& count
)
{
    auto SliceStreamPtr = SliceReading{}.createStream();
    SliceStreamPtr->access(type, pathname);
    SliceStreamPtr->get(blockId, container, start, count);
    SliceStreamPtr->bufferSync();
}


#endif
