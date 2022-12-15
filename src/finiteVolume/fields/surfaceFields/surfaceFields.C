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

#include "surfaceFields.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

defineTemplateTypeNameAndDebug(surfaceScalarField::DimensionedInternalField, 0);
defineTemplateTypeNameAndDebug(surfaceVectorField::DimensionedInternalField, 0);
defineTemplateTypeNameAndDebug(surfaceSphericalTensorField::DimensionedInternalField, 0);
defineTemplateTypeNameAndDebug(surfaceSymmTensorField::DimensionedInternalField, 0);
defineTemplateTypeNameAndDebug(surfaceSymmTensor4thOrderField::DimensionedInternalField, 0);
defineTemplateTypeNameAndDebug(surfaceDiagTensorField::DimensionedInternalField, 0);
defineTemplateTypeNameAndDebug(surfaceTensorField::DimensionedInternalField, 0);

defineTemplateTypeNameAndDebug(surfaceScalarField, 0);
defineTemplateTypeNameAndDebug(surfaceVectorField, 0);
defineTemplateTypeNameAndDebug(surfaceSphericalTensorField, 0);
defineTemplateTypeNameAndDebug(surfaceSymmTensorField, 0);
defineTemplateTypeNameAndDebug(surfaceSymmTensor4thOrderField, 0);
defineTemplateTypeNameAndDebug(surfaceDiagTensorField, 0);
defineTemplateTypeNameAndDebug(surfaceTensorField, 0);

typedef OFCstream<fvsPatchField, surfaceMesh> surfaceOFCstream;
defineTemplateTypeNameAndDebug(surfaceOFCstream, 0);


template<>
Foam::Ostream&
Foam::OFCstream<fvsPatchField, surfaceMesh>::write
(
    const char* data,
    std::streamsize byteSize
)
{
    if (OFstream::debug)
    {
        InfoInFunction
            << "surfaceMesh specialization with feldId_ = "
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
        Info<< "Surface internal field needs to be mapped and staged here\n";
    }
    else if (fieldId_ >= 0)  // Boundary field
    {
        Info<< "A surface boundary field needs to be mapped and staged here\n";
    }
    // else
    // {
        // Write as local array
        Info<< "Writing a surface field as local variable\n";

        fieldId_ = -2;
        adiosWritePrimitives
        (
            "fields",
            this->getBlockId(),
            localTotalSize,
            reinterpret_cast<const scalar*>(data)
        );

        return *this;
    // }
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //
