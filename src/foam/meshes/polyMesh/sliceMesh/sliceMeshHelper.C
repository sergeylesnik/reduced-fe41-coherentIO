
#include "sliceMeshHelper.H"

// * * * * * * * * * * * * * * Free Functions * * * * * * * * * * * * * * * //

void Foam::partitionByFirst(Foam::pairVector<Foam::label, Foam::label>& input)
{
    std::stable_partition
    (
        input.begin(),
        input.end(),
        [](const std::pair<Foam::label, Foam::label>& n)
        {
            return n.first>0;
        }
    );
    std::stable_sort
    (
        std::partition_point
        (
            input.begin(),
            input.end(),
            [](const std::pair<Foam::label, Foam::label>& n)
            {
                return n.first>0;
            }
        ),
        input.end(),
        []
        (
            const std::pair<Foam::label, Foam::label>& n,
            const std::pair<Foam::label, Foam::label>& m
        )
        {
            return n.first>m.first;
        }
    );
}


void
Foam::renumberFaces(Foam::faceList& faces, const std::vector<Foam::label>& map)
{
    std::for_each
    (
        faces.begin(),
        faces.end(),
        [&map](Foam::face& input)
        {
            std::transform
            (
                input.begin(),
                input.end(),
                input.begin(),
                [&map](const Foam::label& id)
                {
                    return map[id];
                }
            );
        }
    );
}


std::vector<Foam::label>
Foam::permutationOfSorted(const Foam::labelList& input)
{
    std::vector<Foam::label> indices{};
    indexIota(indices, input.size(), 0);
    indexSort(indices, input);
    return indices;
}

// ************************************************************************* //
