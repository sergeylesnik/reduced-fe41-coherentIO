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

\*---------------------------------------------------------------------------*/

#include "volFields.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

defineTemplateTypeNameAndDebug(volScalarField::DimensionedInternalField, 0);
defineTemplateTypeNameAndDebug(volVectorField::DimensionedInternalField, 0);
defineTemplateTypeNameAndDebug(volSphericalTensorField::DimensionedInternalField, 0);
defineTemplateTypeNameAndDebug(volSymmTensorField::DimensionedInternalField, 0);
defineTemplateTypeNameAndDebug(volSymmTensor4thOrderField::DimensionedInternalField, 0);
defineTemplateTypeNameAndDebug(volDiagTensorField::DimensionedInternalField, 0);
defineTemplateTypeNameAndDebug(volTensorField::DimensionedInternalField, 0);

defineTemplateTypeNameAndDebug(volScalarField, 0);
defineTemplateTypeNameAndDebug(volVectorField, 0);
defineTemplateTypeNameAndDebug(volSphericalTensorField, 0);
defineTemplateTypeNameAndDebug(volSymmTensorField, 0);
defineTemplateTypeNameAndDebug(volSymmTensor4thOrderField, 0);
defineTemplateTypeNameAndDebug(volDiagTensorField, 0);
defineTemplateTypeNameAndDebug(volTensorField, 0);

typedef OFCstream<fvPatchField, volMesh> volOFCstream;
defineTemplateTypeNameAndDebug(volOFCstream, 0);

template<>
tmp<GeometricField<scalar, fvPatchField, volMesh> >
GeometricField<scalar, fvPatchField, volMesh>::component
(
    const direction
) const
{
    return *this;
}

template<>
void GeometricField<scalar, fvPatchField, volMesh>::replace
(
    const direction,
    const GeometricField<scalar, fvPatchField, volMesh>& gsf
)
{
    *this == gsf;
}

template<>
Foam::Ostream&
Foam::OFCstream<fvPatchField, volMesh>::write
(
    const char* data,
    std::streamsize byteSize
)
{
    if (OFstream::debug)
    {
        InfoInFunction
            << "volMesh specialization with fieldId_ = "
            << fieldId_ << "\n";
    }

    label globalCellSize;
    label cellOffset;
    label nCmpts;
    const label localTotalSize = byteSize/sizeof(scalar);

    if(fieldId_ == -1)  // Internal field
    {
        // cellOffsets is a nProc long list, where each entry represents the
        // sum of cell sizes of the current proc and all the procs below.
        const Offsets& co = sliceableMesh_.cellOffsets();
        globalCellSize = co.upperBound(Pstream::nProcs() - 1);
        cellOffset = co.lowerBound(Pstream::myProcNo());
        nCmpts = localTotalSize/co.count(Pstream::myProcNo());
    }
    else if (fieldId_ >= 0)  // Boundary field
    {
        const globalIndex& bgi = sliceableMesh_.boundaryGlobalIndex(fieldId_);
        globalCellSize = bgi.size();
        cellOffset = bgi.offset(Pstream::myProcNo());
        nCmpts = localTotalSize/bgi.localSize();
    }
    else
    {
        // Write as local array
        adiosWritePrimitives
        (
            "fields",
            this->getBlockId(),
            byteSize/sizeof(scalar),
            reinterpret_cast<const scalar*>(data)
        );

        return *this;
    }

    writeGlobalField
    (
        nCmpts*globalCellSize,
        nCmpts*cellOffset,
        localTotalSize,
        reinterpret_cast<const scalar*>(data)
    );

    return *this;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //
