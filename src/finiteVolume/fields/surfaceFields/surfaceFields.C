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
void Foam::OFCstream<fvsPatchField, surfaceMesh>::removeProcPatchesFromDict()
{
    const Offsets& iso = sliceableMesh_.internalSurfaceFieldOffsets();
    const label localTotalSize = iso.count(Pstream::myProcNo());

    const label nAllPatches = sliceableMesh_.mesh().boundaryMesh().size();
    const label nProcPatches = sliceableMesh_.procPatches().size();
    const label nNonProcPatches = nAllPatches - nProcPatches;

    consolidatedData_.resize(localTotalSize);

    // Store internal and processor patch fields pointers in a list
    List<const fieldDataEntry*> procPatchFDEPtrs(nProcPatches + 1);
    procPatchFDEPtrs[0] =
        dynamic_cast<const fieldDataEntry*>
        (
            dict_.lookupEntryPtr("internalField", false, false)
        );

    dictionary& bfDict = dict_.subDict("boundaryField");
    forAll(sliceableMesh_.procPatches(), i)
    {
        const label ppI = i + 1;
        const dictionary& ppDict =
            bfDict.subDict(sliceableMesh_.procPatches()[i].name());

        procPatchFDEPtrs[ppI] =
            dynamic_cast<const fieldDataEntry*>
            (
                ppDict.lookupEntryPtr("value", false, false)
            );
    }

    // Initialize iterator for each subfield
    labelList fieldIters(procPatchFDEPtrs.size(), 0);

    // processorFaces
    const labelList& pf = sliceableMesh_.internalFaceIDsFromBoundaries();
    // processorFacesPatchIds
    const labelList& pfpi = sliceableMesh_.boundryIDsFromInternalFaces();

    label j = 0;
    forAll(consolidatedData_, i)
    {
        label id;

        if (!pf.empty() && i == pf[j])  // Processor field
        {
            id = pfpi[j] - nNonProcPatches;
            j++;
        }
        else  // Internal field
        {
            id = 0;
        }

        consolidatedData_[i] = *(procPatchFDEPtrs[id]->data() + fieldIters[id]);
        fieldIters[id]++;
    }

    // Create new entry for the consolidated internalField
    fieldDataEntry* newInternal =
    new fieldDataEntry
    (
        "internalField",
        procPatchFDEPtrs[0]->compoundToken(),
        consolidatedData_.data(),
        localTotalSize*sizeof(scalar),
        localTotalSize/procPatchFDEPtrs[0]->nCmpts()
    );

    // Set the new internalField in the dictionary replacing the old one
    dict_.set(newInternal);

    // Remove processor patches from the dictionary
    forAll(sliceableMesh_.procPatches(), i)
    {
        word ppName = sliceableMesh_.procPatches()[i].name();
        bfDict.remove(ppName);
    }
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //
