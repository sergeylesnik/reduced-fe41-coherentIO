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
    Foam::ProcessorPatch

Description
    Required for processor patch construction on coherent mesh layout.

Author
    Gregor Weiss, HLRS University of Stuttgart, 2023
    Sergey Lesnik, Wikki GmbH, 2023
    Henrik Rusche, Wikki GmbH, 2023

\*---------------------------------------------------------------------------*/

#include "ProcessorPatch.H"

#include "Offsets.H"

#include <set>

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

Foam::label Foam::ProcessorPatch::instanceCount_ = 0;

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::ProcessorPatch::ProcessorPatch
(
    const Foam::Slice& slice,
    const Foam::labelList& neighbours,
    const Foam::label& numBoundaries
)
:
    numNonProcPatches_{numBoundaries},
    slice_{slice},
    localFaceIDs_
    {
        labelList
        (
            std::count_if
            (
                neighbours.begin(),
                neighbours.end(),
                slice
            )
        )
    },
    procBoundaryName_
    {
        word("procBoundary") +
        Foam::name(Pstream::myProcNo()) +
        word("to") +
        Foam::name(slice_.partition())
    }
{
    determineFaceIDs(neighbours);
    ++instanceCount_;
    id_ = Foam::encodeSlicePatchId((numNonProcPatches_ - 1) + instanceCount_);
}


// Copy constructor
Foam::ProcessorPatch::ProcessorPatch(const ProcessorPatch& other)
:
    id_{other.id_},
    slice_{other.slice_},
    localFaceIDs_{other.localFaceIDs_.begin(), other.localFaceIDs_.end()},
    localPointIDs_{other.localPointIDs_.begin(), other.localPointIDs_.end()},
    procBoundaryName_{other.procBoundaryName_}
{
    ++instanceCount_;
}


//TODO: Must not be copyable because of instanceCount_ consider move semantics only
Foam::ProcessorPatch&
Foam::ProcessorPatch::operator=(Foam::ProcessorPatch const& other)
{
    Foam::ProcessorPatch tmp(other);
    swap(tmp);
    return *this;
}

// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * //

Foam::ProcessorPatch::~ProcessorPatch()
{
    --instanceCount_;
}

// * * * * * * * * * * * * Public Member Functions * * * * * * * * * * * * //

void Foam::ProcessorPatch::swap(Foam::ProcessorPatch& other) noexcept
{
    using std::swap;
    swap( id_, other.id_ );
    swap( instanceCount_, other.instanceCount_ );
    swap( slice_, other.slice_ );
    swap( localFaceIDs_, other.localFaceIDs_ );
    swap( localPointIDs_, other.localPointIDs_ );
    swap( procBoundaryName_, other.procBoundaryName_ );
}


Foam::label Foam::ProcessorPatch::id() const
{
    return id_;
}


Foam::word Foam::ProcessorPatch::name() const
{
    return procBoundaryName_;
}


Foam::label Foam::ProcessorPatch::partner() const
{
    return slice_.partition();
}


void Foam::ProcessorPatch::determineFaceIDs(const Foam::labelList& neighbours)
{
    auto faceIdIter = localFaceIDs_.begin();
    for
    (
        auto neighboursIter = std::begin(neighbours);
        neighboursIter != std::end(neighbours);
        ++neighboursIter
    )
    {
        if (slice_(*neighboursIter))
        {
            *faceIdIter = std::distance(neighbours.begin(), neighboursIter);
            ++faceIdIter;
        }
    }
}


void Foam::ProcessorPatch::determinePointIDs
(
    const Foam::faceList& faces,
    const Foam::label& bottomPointId
)
{
    auto pred = [bottomPointId](const Foam::label& id)
                {
                    return bottomPointId<=id;
                };
    std::set<Foam::label> pointIDs{};
    Foam::subset
    (
        faces.begin(),
        faces.end(),
        std::inserter(pointIDs, pointIDs.end()),
        pred
    );
    localPointIDs_.resize(pointIDs.size());
    std::transform
    (
        pointIDs.begin(),
        pointIDs.end(),
        localPointIDs_.begin(),
        [&bottomPointId](const Foam::label& id)
        {
            return id - bottomPointId;
        }
    );
}


void Foam::ProcessorPatch::appendOwner
(
    Foam::labelList& owner,
    Foam::labelList& recvOwner
)
{
    std::transform
    (
        recvOwner.begin(),
        recvOwner.end(),
        recvOwner.begin(),
        [this](const Foam::label& id)
        {
            return slice_.convert(id);
        }
    );
    owner.append(recvOwner);
}


void Foam::ProcessorPatch::encodePatch(Foam::labelList& neighbours)
{
    std::transform
    (
        std::begin(neighbours),
        std::end(neighbours),
        std::begin(neighbours),
        [this](const Foam::label& cellId)
        {
            return slice_(cellId) ? id_ : cellId;
        }
    );
}


void Foam::ProcessorPatch::encodePatch
(
    std::vector<Foam::label>& neighbours,
    const Foam::label& numPatchFaces
)
{
    std::fill_n(std::back_inserter(neighbours), numPatchFaces, id_);
}


void Foam::ProcessorPatch::encodePatch
(
    Foam::labelList& neighbours,
    const Foam::label& numPatchFaces
)
{
    auto curr_size = neighbours.size();
    neighbours.resize(curr_size + numPatchFaces);
    std::fill_n(neighbours.begin() + curr_size, numPatchFaces, id_);
}


Foam::pointField
Foam::ProcessorPatch::extractPoints(const Foam::pointField& input)
{
    Foam::pointField output;
    output.resize(localPointIDs_.size());
    std::transform
    (
        std::begin(localPointIDs_),
        std::end(localPointIDs_),
        std::begin(output),
        [&input](const Foam::label& id)
        {
            return input[id];
        }
    );
    return output;
}


void Foam::swap(Foam::ProcessorPatch& a, Foam::ProcessorPatch& b) noexcept
{
    a.swap( b );
}


// ************************************************************************* //
