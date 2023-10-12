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

#include "nonblockConsensus.H"

#include "Offsets.H"
#include "Slice.H"


std::map<Foam::label, Foam::label>
Foam::nonblockConsensus(const std::map<Foam::label, Foam::label>& data)
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
            &(msg.second),
            1,
            MPI_INT,
            msg.first,
            tag,
            MPI_COMM_WORLD,
            &request
        );
        issRequests.push_back(request);
    }

    std::map<Foam::label, Foam::label> recvBuffer{};
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
            MPI_Get_count(&status, MPI_INT, &count);
            Foam::label numberOfPartitionFaces{};
            MPI_Recv
            (
                &numberOfPartitionFaces,
                count,
                MPI_INT,
                status.MPI_SOURCE,
                tag,
                MPI_COMM_WORLD,
                MPI_STATUS_IGNORE
            );
            recvBuffer[status.MPI_SOURCE] = numberOfPartitionFaces;
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

// ************************************************************************* //
