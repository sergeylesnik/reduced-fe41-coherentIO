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
    Pout<< "WeAreNowSpecial\n";
    if(fieldId_ == 0)
    {
        const label localSize = byteSize/sizeof(scalar);

        // Ensure that "processor.*" is not in the id
        fileName localId = this->getBlockId();
        wordList cmpts = localId.components();
        fileName globalId;
        forAll(cmpts, i)
        {
            if (cmpts[i].find("processor") == std::string::npos)
            {
                globalId += cmpts[i] + '/';
            }
        }

        // cellOffsets is a nProc long list, where each entry represents the
        // sum of cell sizes of the current proc and all the procs below.
        label globalCellSize =
            sliceableMesh_.cellOffsets().upperBound(Pstream::nProcs() - 1);
        label cellOffset =
            sliceableMesh_.cellOffsets().lowerBound(Pstream::myProcNo());
        label localCellSize =
            sliceableMesh_.cellOffsets().count(Pstream::myProcNo());
        label nCmpts = localSize/localCellSize;

        adiosWritePrimitives
        (
            "fields",
            globalId,
            nCmpts*globalCellSize,
            nCmpts*cellOffset,
            nCmpts*localCellSize,
            reinterpret_cast<const scalar*>(data)
        );
    }
    else
    {
        adiosWritePrimitives
        (
            "fields",
            this->getBlockId(),
            byteSize/sizeof(scalar),
            reinterpret_cast<const scalar*>(data)
        );
    }

    return *this;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //
