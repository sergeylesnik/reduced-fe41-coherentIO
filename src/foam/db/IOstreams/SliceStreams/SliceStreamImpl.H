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

#include "adios2.h"

#include "SliceStream.H"

#include "variableBuffer.H"
#include "spanBuffer.H"


template<typename BufferType>
std::shared_ptr<Foam::SliceBuffer>
createInputBuffer
(
    adios2::IO* const ioPtr,
    adios2::Engine* const enginePtr,
    const Foam::string& blockId,
    const Foam::labelList& start,
    const Foam::labelList& count
)
{
    if (!start.empty() && !count.empty())
    {
        return std::make_shared<BufferType>
               (
                   ioPtr,
                   enginePtr,
                   blockId,
                   start,
                   count
               );
    }
    else
    {
        return std::make_shared<BufferType>
               (
                   ioPtr,
                   enginePtr,
                   blockId
               );
    }
}


struct Foam::SliceStream::Impl
{

    Impl() = default;

    std::shared_ptr<SliceBuffer> bufferPtr_{nullptr};

    template<typename BufferType>
    label readingBuffer
    (
        adios2::IO* const ioPtr,
        adios2::Engine* const enginePtr,
        const Foam::string& blockId,
        const labelList& start = {},
        const labelList& count = {}
    )
    {
        std::shared_ptr<SliceBuffer> bufferPtr{nullptr};
        if (ioPtr && enginePtr)
        {
            bufferPtr = createInputBuffer<BufferType>
                        (
                            ioPtr,
                            enginePtr,
                            blockId,
                            start,
                            count
                        );
        }
        bufferPtr_ = bufferPtr;
        return bufferPtr_->size();
    }


    template<typename BufferType>
    void writingBuffer
    (
        adios2::IO* const ioPtr,
        adios2::Engine* const enginePtr,
        const Foam::string& blockId,
        const labelList& shape,
        const labelList& start,
        const labelList& count
    )
    {
        std::shared_ptr<SliceBuffer> bufferPtr{nullptr};
        if (ioPtr && enginePtr)
        {
            bufferPtr = std::make_shared<BufferType>
                        (
                            ioPtr,
                            enginePtr,
                            blockId,
                            shape,
                            start,
                            count
                        );
        }
        bufferPtr_ = bufferPtr;
    }


    // Reading local/global array
    template<class DataType>
    typename std::enable_if<!std::is_const<DataType>::value, void>::type
    get
    (
        adios2::IO* const ioPtr,
        adios2::Engine* const enginePtr,
        const Foam::string& blockId,
        DataType* data,
        const labelList& start,
        const labelList& count
    )
    {
        readingBuffer<variableBuffer<DataType>>
        (
            ioPtr,
            enginePtr,
            blockId,
            start,
            count
        );
        if (bufferPtr_)
        {
            bufferPtr_->transfer(enginePtr, data);
        }
    }


    // Reading local/global array
    template<class ContainerType>
    typename std::enable_if<!std::is_const<ContainerType>::value, void>::type
    get
    (
        adios2::IO* const ioPtr,
        adios2::Engine* const enginePtr,
        const Foam::string& blockId,
        ContainerType& container,
        const labelList& start,
        const labelList& count
    )
    {
        typedef typename ContainerType::value_type DataType;
        auto size = readingBuffer<variableBuffer<DataType>>
                    (
                        ioPtr,
                        enginePtr,
                        blockId,
                        start,
                        count
                    );
        container.resize(size);
        if (bufferPtr_)
        {
            bufferPtr_->transfer(enginePtr, container.data());
        }
    }


    // Writing local/global array
    template<class DataType>
    void put
    (
        adios2::IO* const ioPtr,
        adios2::Engine* const enginePtr,
        const Foam::string& blockId,
        const labelList& shape,
        const labelList& start,
        const labelList& count,
        const DataType* data,
        const labelList& mapping = {},
        const bool masked = false
    )
    {
        if (mapping.empty())
        {
            writingBuffer<variableBuffer<DataType>>
            (
                ioPtr,
                enginePtr,
                blockId,
                shape,
                start,
                count
            );
            bufferPtr_->transfer(enginePtr, data);
        }
        else
        {
            writingBuffer<spanBuffer<DataType>>
            (
                ioPtr,
                enginePtr,
                blockId,
                shape,
                start,
                count
            );
            bufferPtr_->transfer(enginePtr, data, mapping, masked);
        }
    }
};

