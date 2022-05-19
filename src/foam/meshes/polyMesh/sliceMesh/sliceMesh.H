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
    Foam::sliceMesh

Description
    Foam::sliceMesh

SourceFiles
    sliceMesh.C

\*---------------------------------------------------------------------------*/

#ifndef sliceMesh_H
#define sliceMesh_H

#include "sliceMeshHelper.H"
#include "polyMesh.H"

#include <vector>
#include <algorithm>

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

// Forward declaration
class pointMesh;

label encodeSlicePatchId( const label& );
label decodeSlicePatchId( const label& );
label sliceNeighbourId( const label&, const polyMesh&);

/*---------------------------------------------------------------------------*\
                       Class sliceMesh Declaration
\*---------------------------------------------------------------------------*/


class sliceMesh
{
    std::vector<std::pair<label, label> > polyNeighboursPermutation_{};
    std::vector<label> polyNeighboursAndPatches_{};

    std::vector<label> permutationToPoly_{};
    std::vector<label> permutationToSlice_{};

    std::vector<label> permutationToSlicePoint_{};
    std::vector<label> permutationToPolyPoint_{};

    std::vector<std::pair<label, label> >
    createPolyNeighbourPermutation( const std::vector<label>& );

    std::vector<label> createSlicePermutation( const labelList& );

    void createPointPermutation( faceList&, const label& );

    void renumberToSlice( faceList& );

    template< typename IterType >
    void resetNextPatch( IterType& patchBegin,
                         IterType& patchEnd,
                         polyPatch& patch,
                         const label& patchId ) {
        findValueExtend( patchBegin,
                         patchEnd,
                         polyNeighboursAndPatches_.end(),
                         encodeSlicePatchId( patchId ) );
        patch.resetPatch( std::distance( patchBegin, patchEnd ),
                          std::distance( polyNeighboursAndPatches_.begin(), patchBegin ) );
    }

    auto findPatchBegin() {
        return std::find_if( polyNeighboursAndPatches_.begin(),
                             polyNeighboursAndPatches_.end(),
                             []( const auto& id )  
                             { return id == encodeSlicePatchId( 0 ); } );
    }


public:

    // Constructors
    sliceMesh() = delete;

    //- Construct from sliceMesh neighbour and patch list
    explicit sliceMesh( const std::vector<label>& );

    //- Construct from polyMesh
    explicit sliceMesh( const polyMesh& );

    //- Construct from faces, owners and number of points
    sliceMesh( const labelList&, const faceList&, const label& );

    // Destructor
    ~sliceMesh() = default;

    // Member Functions
    //

    // Transformations to sliceMesh
    
    template<typename Container>
    void mapToSlice( Container& );

    void mapToSlice( pointField& );

    label mapToSlice( const label& );

    template<typename MeshElement>
    MeshElement generateSlice( MeshElement& );

    faceList generateSlice( faceList& );

    // Generate 'sliced' neighbours
    labelList generateSlice( labelList&, const polyMesh& );

    // Transformations to polyMesh
    
    template<typename Container>
    void mapToPoly( Container& );
    
    label mapToPoly( const label& );

    void resetPolyPatches( polyBoundaryMesh& );

    template<typename Container>
    void copyPolyNeighbours( Container& );
};

// Transformations to sliceMesh

template<typename Container>
void Foam::sliceMesh::mapToSlice( Container& input ) {
    applyPermutation( input, permutationToSlice_ );
}


template<typename MeshElement>
MeshElement Foam::sliceMesh::generateSlice( MeshElement& input ) {
    mapToSlice( input );
    return input;
}


// Transformations to polyMesh

template<typename Container>
void Foam::sliceMesh::mapToPoly( Container& input ) {
    applyPermutation( input, permutationToPoly_ );
}


template<typename Container>
void Foam::sliceMesh::copyPolyNeighbours( Container& neighbours ) {
     const auto polyNeighboursBegin = polyNeighboursAndPatches_.begin();
     const auto polyPatchBegin = findPatchBegin();
     neighbours.resize( std::distance( polyNeighboursBegin, polyPatchBegin ) );
     std::copy( polyNeighboursBegin, polyPatchBegin, neighbours.begin() );
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
