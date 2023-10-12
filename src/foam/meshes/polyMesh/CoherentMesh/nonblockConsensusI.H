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

Function
    Foam::nonblockConsensus

Description
    Implements the nonblocking concensus exchange algorithm.
    @article
    {
        doi = {10.1145/1837853.1693476},
        author = {Hoefler, Torsten and Siebert, Christian and Lumsdaine, Andrew},
        title = {Scalable Communication Protocols for Dynamic Sparse Data Exchange},
        year = {2010},
        volume = {45},
        number = {5},
        journal = {SIGPLAN Not.},
        pages = {159–168}
    }

Author
    Gregor Weiss, HLRS University of Stuttgart, 2023
    Sergey Lesnik, Wikki GmbH, 2023
    Henrik Rusche, Wikki GmbH, 2023

\*---------------------------------------------------------------------------*/


template<class Type>
std::map<Foam::label, std::vector<Type>>
Foam::nonblockConsensus
(
    const std::map<Foam::label,
    std::vector<Type>>& data,
    MPI_Datatype dtype
)
{
    int tag = 314159;
    bool barrier_activated = false;
    MPI_Barrier(MPI_COMM_WORLD);
    std::vector<MPI_Request> issRequests{};
    for (const auto& msg: data)
    {
        MPI_Request request;
        MPI_Issend
        (
            &(msg.second[0]),
            msg.second.size(),
            dtype,
            msg.first,
            tag,
            MPI_COMM_WORLD,
            &request
        );
        issRequests.push_back(request);
    }

    std::map<Foam::label, std::vector<Type>> recvBuffer{};
    MPI_Request barrier = MPI_REQUEST_NULL;
    int done = 0;
    while (!done)
    {
        int flag = 0;
        int count = 0;
        MPI_Status status;
        MPI_Iprobe(MPI_ANY_SOURCE, tag, MPI_COMM_WORLD, &flag, &status);
        if (flag)
        {
            MPI_Get_count(&status, dtype, &count);
            std::vector<Type> recvMessage(count);
            MPI_Recv
            (
                &(recvMessage[0]),
                count,
                dtype,
                status.MPI_SOURCE,
                tag,
                MPI_COMM_WORLD,
                MPI_STATUS_IGNORE
            );
            recvBuffer[status.MPI_SOURCE] = std::move(recvMessage);
        }

        if (barrier_activated)
        {
            MPI_Test(&barrier, &done, MPI_STATUS_IGNORE);
        }
        else
        {
            int sent;
            MPI_Testall
            (
                issRequests.size(),
                issRequests.data(),
                &sent,
                MPI_STATUSES_IGNORE
            );
            if (sent)
            {
                MPI_Ibarrier(MPI_COMM_WORLD, &barrier);
                barrier_activated = true;
            }
        }
    }

    return recvBuffer;
}
