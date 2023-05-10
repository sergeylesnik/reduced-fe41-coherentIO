
// * * * * * * * * * * * * * * Free Functions  * * * * * * * * * * * * * * //

template<typename SomeList, typename IndexList>
SomeList Foam::extractor(const SomeList& input, const IndexList& extractorList)
{
    SomeList output;
    output.resize(extractorList.size());
    std::transform
    (
        std::begin(extractorList),
        std::end(extractorList),
        std::begin(output),
        [&input](const auto& id)
        {
            return input[id];
        }
    );
    return output;
}


template<typename Container>
Foam::label Foam::countNeighOfSlice
(
    const Container& neighbours,
    const Foam::Slice& slice
)
{
    return std::count_if(neighbours.begin(), neighbours.end(), slice);
}

// * * * * * * * * * * * * Public Member Functions * * * * * * * * * * * * //

template<typename FaceList>
FaceList Foam::sliceProcPatch::extractFaces(const FaceList& input)
{
    return Foam::extractor(input, localFaceIDs_);
}

// ************************************************************************* //