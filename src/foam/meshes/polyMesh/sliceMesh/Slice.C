
#include "Slice.H"

#include "Offsets.H"

Foam::Slice::Slice( const Foam::label& partition,
                    const Foam::Offsets& cellOffsets,
                    const Foam::Offsets& pointOffsets )
    : partition_{ partition }
    , lowerCellId_{ cellOffsets.lowerBound( partition ) }
    , upperCellId_{ cellOffsets.upperBound( partition ) } 
    , lowerPointId_{ pointOffsets.lowerBound( partition ) } 
    , upperPointId_{ pointOffsets.upperBound( partition ) } 
    , bottom_{ lowerCellId_ }
    , top_{ lowerCellId_ }
    , cellMap_{ std::make_shared<Foam::sliceMap>( cellOffsets.count( partition ) ) }
    , pointMap_{ std::make_shared<Foam::sliceMap>( pointOffsets.count( partition ) ) }
    , mapping_{ cellMap_ }
{}

