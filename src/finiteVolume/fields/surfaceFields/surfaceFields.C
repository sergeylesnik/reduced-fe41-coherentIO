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
    scalarList coherentData;

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
            compToken.resize(mesh.nInternalFaces());

            // Store the data pointer in a UList for convenience
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

            coherentData.resize(localSize);

            adiosStreamPtr_->open("fields", pathname_.path());
            adiosStreamPtr_->transfer
            (
                id,
                reinterpret_cast<scalar*>(coherentData.data()),
                List<label>({nCmpts*elemOffset}),
                List<label>({nCmpts*nElems})
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
                        bfDict.add(patchName, dict);
                    }
                }

                return dict_;
            }
        }
    }


    // Create dictionary entries for the processor patch fields with the
    // correctly sized compound tokens

    const label nAllPatches = mesh.boundaryMesh().size();

    List<UList<scalar> > procPatchData(nAllPatches);
    const polyBoundaryMesh& bm = sliceableMesh_.mesh().boundaryMesh();
    label nProcPatches = 0;

    forAll(bm, patchI)
    {
        const polyPatch& patch = bm[patchI];

        if (isA<processorPolyPatch>(patch))
        {
            const label patchSize = patch.size();

            tokenList entryTokens(2);
            entryTokens[0] = word("nonuniform");

            // Create a compound token for the patch, resize and store in a list
            autoPtr<token::compound> ctPtr =
                token::compound::New("List<" + fieldTypeName + ">", patchSize);

            // Store the data pointer for the later mapping
            procPatchData[nProcPatches++] =
                UList<scalar>(reinterpret_cast<scalar*>(ctPtr().data()), patchSize);

            // autoPtr is invalid after calling ptr()
            entryTokens[1] = ctPtr.ptr();
            dictionary dict;
            dict.add("type", "processor");
            // Xfer needed here, calls the corresponding primitiveEntry
            // constructor
            dict.add("value", entryTokens.xfer());
            // No Xfer implemented for adding dictionary but copying is
            // accomplished by cloning the pointers of the hash table and
            // IDLList
            bfDict.add(patch.name(), dict);
        }
    }

    procPatchData.resize(nProcPatches);
    const label nNonProcPatches = nAllPatches - nProcPatches;


    // Map from the coherent format to the internal and processor patch fields

    // processorFaces
    const labelList& pf = sliceableMesh_.internalFaceIDsFromBoundaries();

    // processorFacesPatchIds
    const labelList& pfpi = sliceableMesh_.boundryIDsFromInternalFaces();

    labelList patchFaceI(nProcPatches, 0);
    label internalFaceI = 0;
    label pfI = 0;

    // Closing the IO engine ensures that the data is read from disk
    adiosStreamPtr_->close();

    if (pf.empty())
    {
        // Use explicit assign(). An assignment operator would trigger a
        // shallow copy by setting internalData data pointer to that of
        // coherentData. But the latter is destroyed after returning and the
        // data pointer would not be valid anymore.
        internalData.assign(coherentData);
    }
    else
    {
        forAll(coherentData, i)
        {
            if (i < pf.size() && i == pf[pfI])  // Processor field
            {
                const label patchI = pfpi[pfI++] - nNonProcPatches;
                procPatchData[patchI][patchFaceI[patchI]++] = coherentData[i];
            }
            else  // Internal field
            {
                internalData[internalFaceI++] = coherentData[i];
            }
        }
    }

    return dict_;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //
