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

    // List<UList<scalar> > patchData(nProcPatches);
    // labelList patchFaceI(nProcPatches, 0);
    // UList<scalar> internalData(SIZE);
    // label internalFaceI = 0;
    // if (pf.empty())
    // {
    //     consolidatedData_ = internalData;
    // }
    // else
    // {
    //     forAll(consolidatedData_, i)
    //     {
    //         if (i == pf[j])  // Processor field
    //         {
    //             label id = pfpi[j] - nNonProcPatches;
    //             consolidatedData_[i] = patchData[patchFaceI[id]++];
    //         }
    //         else  // Internal field
    //         {
    //             consolidatedData_[i] = internalData[internalFaceI++];
    //         }
    //     }
    // }

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

template<>
label IFCstream::coherentFieldSize<fvsPatchField, surfaceMesh>()
{
    return sliceableMesh_.internalSurfaceFieldOffsets().size();
}

template<>
dictionary& IFCstream::readToDict<fvsPatchField, surfaceMesh>
(
    const word& fieldTypeName
)
{
    // Internal field data
    UList<scalar> internalData;

    // Field data in the coherent format holding internal and processor patch
    // fields
    scalarList coherentFieldData;

    // Fill the dictionary with the stream
    dict_.read(*this);

    dictionary& bfDict = dict_.subDict("boundaryField");
    ITstream& is = dict_.lookup("internalField");
    const polyMesh& mesh = sliceableMesh_.mesh();

    // Traverse the tokens of the internal field entry
    while (!is.eof())
    {
        token currToken(is);

        if (currToken.isCompound())
        {
            // Resize the compoundToken according to the mesh of the proc and
            // read the corresponding slice of the data.

            token::compound& compToken = currToken.compoundToken();

            internalData = UList<scalar>
            (
                reinterpret_cast<scalar*>(compToken.data()),
                mesh.nInternalFaces()
            );

            // Current token index points to the token after the compound
            // ToDoIO Get rid of globalSize?
            const label globalSize = is[is.tokenIndex()++].labelToken();
            const string id = is[is.tokenIndex()++].stringToken();

            const label localSize =
                coherentFieldSize<fvsPatchField, surfaceMesh>();
            const globalIndex gi(localSize);
            const label elemOffset = gi.offset(Pstream::myProcNo());
            const label nElems = gi.localSize();
            const label nCmpts = compToken.nComponents();

            coherentFieldData.resize(localSize);

            adiosReadPrimitives
            (
                "fields",
                id,
                reinterpret_cast<scalar*>(coherentFieldData.data()),
                nCmpts*elemOffset,
                nCmpts*nElems
            );
        }
        else if (currToken.isWord())
        {
            if (currToken.wordToken() == "uniform")
            {
                // Both field types are uniform. Create processor patch fields
                // with the same value entry as the internal field.

                forAll(mesh.boundaryMesh(), i)
                {
                    const polyPatch& patch = mesh.boundaryMesh()[i];
                    const word& patchName = patch.name();

                    if (patch.type() != processorPolyPatch::typeName)
                    {
                        dictionary& patchDict = bfDict.subDict(patchName);
                        readCompoundTokenData(patchDict, patch.size());
                    }
                    else
                    {
                        dictionary dict;
                        dict.add("type", "processor");
                        dict.add("value", is.xfer());
                        bfDict.add(patchName, dict.xfer());
                    }
                }

                return dict_;
            }
        }
    }


    // Create dictionary entries with the correctly sized compound tokens

    const label nAllPatches = mesh.boundaryMesh().size();
    const label nProcPatches = sliceableMesh_.procPatches().size();
    const label nNonProcPatches = nAllPatches - nProcPatches;

    List<UList<scalar> > patchData(nProcPatches);

    forAll(sliceableMesh_.procPatches(), i)
    {
        const word& patchName = sliceableMesh_.procPatches()[i].name();
        const label patchSize = sliceableMesh_.procPatches()[i].size();

        tokenList entryTokens(3);
        entryTokens[0] = word("nonuniform");
        entryTokens[1] = patchSize;

        // Create a compound token for the patch, resize and store in a list
        autoPtr<token::compound> ctPtr =
            token::compound::New(fieldTypeName, *this, patchSize);

        // Store the data pointer for the later mapping
        patchData[i] =
            UList<scalar>(reinterpret_cast<scalar*>(ctPtr().data()), patchSize);

        // autoPtr is invalid after calling ptr()
        entryTokens[2] = ctPtr.ptr();

        dictionary dict;
        dict.add("type", "processor");
        dict.add("value", entryTokens.xfer());
        bfDict.add(patchName, dict.xfer());
    }


    // Map from the coherent format to the internal and processor patch fields

    // processorFaces
    const labelList& pf = sliceableMesh_.internalFaceIDsFromBoundaries();

    // processorFacesPatchIds
    const labelList& pfpi = sliceableMesh_.boundryIDsFromInternalFaces();

    labelList patchFaceI(nProcPatches, 0);
    label internalFaceI = 0;
    label pfI = 0;

    if (pf.empty())
    {
        internalData = coherentFieldData;
    }
    else
    {
        forAll(coherentFieldData, i)
        {
            if (i == pf[pfI])  // Processor field
            {
                label patchI = pfpi[pfI++] - nNonProcPatches - 1;
                coherentFieldData[i] = patchData[patchI][patchFaceI[patchI]++];
            }
            else  // Internal field
            {
                coherentFieldData[i] = internalData[internalFaceI++];
            }
        }
    }

    return dict_;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //
