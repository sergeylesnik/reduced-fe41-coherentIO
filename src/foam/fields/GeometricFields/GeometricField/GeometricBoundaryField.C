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

#include "emptyPolyPatch.H"
#include "commSchedule.H"
#include "globalMeshData.H"

#include "IOmanip.H"

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template<class Type, template<class> class PatchField, class GeoMesh>
Foam::GeometricField<Type, PatchField, GeoMesh>::GeometricBoundaryField::
GeometricBoundaryField
(
    const BoundaryMesh& bmesh,
    const DimensionedField<Type, GeoMesh>& field,
    const word& patchFieldType
)
:
    FieldField<PatchField, Type>(bmesh.size()),
    bmesh_(bmesh)
{
    if (debug)
    {
        Info<< "GeometricField<Type, PatchField, GeoMesh>::"
               "GeometricBoundaryField::"
               "GeometricBoundaryField(const BoundaryMesh&, "
               "const Field<Type>&, const word&)"
            << endl;
    }

    forAll(bmesh_, patchi)
    {
        this->set
        (
            patchi,
            PatchField<Type>::New
            (
                patchFieldType,
                bmesh_[patchi],
                field
            )
        );
    }
}


template<class Type, template<class> class PatchField, class GeoMesh>
Foam::GeometricField<Type, PatchField, GeoMesh>::GeometricBoundaryField::
GeometricBoundaryField
(
    const BoundaryMesh& bmesh,
    const DimensionedField<Type, GeoMesh>& field,
    const wordList& patchFieldTypes
)
:
    FieldField<PatchField, Type>(bmesh.size()),
    bmesh_(bmesh)
{
    if (debug)
    {
        Info<< "GeometricField<Type, PatchField, GeoMesh>::"
               "GeometricBoundaryField::"
               "GeometricBoundaryField(const BoundaryMesh&, "
               "const Field<Type>&, const wordList&)"
            << endl;
    }

    if (patchFieldTypes.size() != this->size())
    {
        FatalErrorIn
        (
            "GeometricField<Type, PatchField, GeoMesh>::"
            "GeometricBoundaryField::"
            "GeometricBoundaryField(const BoundaryMesh&, "
            "const Field<Type>&, const wordList&)"
        )   << "Incorrect number of patch type specifications given" << nl
            << "    Number of patches in mesh = " << bmesh.size()
            << " number of patch type specifications = "
            << patchFieldTypes.size()
            << abort(FatalError);
    }

    forAll(bmesh_, patchi)
    {
        this->set
        (
            patchi,
            PatchField<Type>::New
            (
                patchFieldTypes[patchi],
                bmesh_[patchi],
                field
            )
        );
    }
}


template<class Type, template<class> class PatchField, class GeoMesh>
Foam::GeometricField<Type, PatchField, GeoMesh>::GeometricBoundaryField::
GeometricBoundaryField
(
    const BoundaryMesh& bmesh,
    const DimensionedField<Type, GeoMesh>& field,
    const PtrList<PatchField<Type> >& ptfl
)
:
    FieldField<PatchField, Type>(bmesh.size()),
    bmesh_(bmesh)
{
    if (debug)
    {
        Info<< "GeometricField<Type, PatchField, GeoMesh>::"
               "GeometricBoundaryField::"
               "GeometricBoundaryField(const BoundaryMesh&, "
               "const Field<Type>&, const PatchField<Type>List&)"
            << endl;
    }

    forAll(bmesh_, patchi)
    {
        this->set(patchi, ptfl[patchi].clone(field));
    }
}


template<class Type, template<class> class PatchField, class GeoMesh>
Foam::GeometricField<Type, PatchField, GeoMesh>::GeometricBoundaryField::
GeometricBoundaryField
(
    const DimensionedField<Type, GeoMesh>& field,
    const typename GeometricField<Type, PatchField, GeoMesh>::
    GeometricBoundaryField& btf
)
:
    FieldField<PatchField, Type>(btf.size()),
    bmesh_(btf.bmesh_)
{
    if (debug)
    {
        Info<< "GeometricField<Type, PatchField, GeoMesh>::"
               "GeometricBoundaryField::"
               "GeometricBoundaryField(const GeometricBoundaryField<Type, "
               "PatchField, BoundaryMesh>&)"
            << endl;
    }

    forAll(bmesh_, patchi)
    {
        this->set(patchi, btf[patchi].clone(field));
    }
}


// Construct as copy
// Dangerous because Field may be set to a field which gets deleted.
// Need new type of GeometricBoundaryField, one which IS part of a geometric
// field for which snGrad etc. may be called and a free standing
// GeometricBoundaryField for which such operations are unavailable.
template<class Type, template<class> class PatchField, class GeoMesh>
Foam::GeometricField<Type, PatchField, GeoMesh>::GeometricBoundaryField::
GeometricBoundaryField
(
    const typename GeometricField<Type, PatchField, GeoMesh>::
    GeometricBoundaryField& btf
)
:
    FieldField<PatchField, Type>(btf),
    bmesh_(btf.bmesh_)
{
    if (debug)
    {
        Info<< "GeometricField<Type, PatchField, GeoMesh>::"
               "GeometricBoundaryField::"
               "GeometricBoundaryField(const GeometricBoundaryField<Type, "
               "PatchField, BoundaryMesh>&)"
            << endl;
    }
}


template<class Type, template<class> class PatchField, class GeoMesh>
Foam::GeometricField<Type, PatchField, GeoMesh>::GeometricBoundaryField::
GeometricBoundaryField
(
    const BoundaryMesh& bmesh,
    const DimensionedField<Type, GeoMesh>& field,
    const dictionary& dict
)
:
    FieldField<PatchField, Type>(bmesh.size()),
    bmesh_(bmesh)
{
    if (debug)
    {
        Info<< "GeometricField<Type, PatchField, GeoMesh>::"
               "GeometricBoundaryField::"
               "GeometricBoundaryField"
               "(const BoundaryMesh&, const Field<Type>&, const dictionary&)"
            << endl;
    }

    forAll(bmesh_, patchi)
    {
        if (bmesh_[patchi].type() != emptyPolyPatch::typeName)
        {
            this->set
            (
                patchi,
                PatchField<Type>::New
                (
                    bmesh_[patchi],
                    field,
                    dict.subDict(bmesh_[patchi].name())
                )
            );
        }
        else
        {
            this->set
            (
                patchi,
                PatchField<Type>::New
                (
                    emptyPolyPatch::typeName,
                    bmesh_[patchi],
                    field
                )
            );
        }
    }
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class Type, template<class> class PatchField, class GeoMesh>
void Foam::GeometricField<Type, PatchField, GeoMesh>::GeometricBoundaryField::
updateCoeffs()
{
    if (debug)
    {
        Info<< "GeometricField<Type, PatchField, GeoMesh>::"
               "GeometricBoundaryField::"
               "updateCoeffs()" << endl;
    }

    forAll(*this, patchi)
    {
        this->operator[](patchi).updateCoeffs();
    }
}


template<class Type, template<class> class PatchField, class GeoMesh>
void Foam::GeometricField<Type, PatchField, GeoMesh>::GeometricBoundaryField::
evaluate()
{
    if (debug)
    {
        Info<< "GeometricField<Type, PatchField, GeoMesh>::"
               "GeometricBoundaryField::"
               "evaluate()" << endl;
    }

    if
    (
        Pstream::defaultComms() == Pstream::blocking
     || Pstream::defaultComms() == Pstream::nonBlocking
    )
    {
        forAll(*this, patchi)
        {
            this->operator[](patchi).initEvaluate
            (
                Pstream::defaultComms()
            );
        }

        // Block for any outstanding requests
        if (Pstream::defaultComms() == Pstream::nonBlocking)
        {
            IPstream::waitRequests();
            OPstream::waitRequests();
        }

        forAll(*this, patchi)
        {
            this->operator[](patchi).evaluate
            (
                Pstream::defaultComms()
            );
        }
    }
    else if (Pstream::defaultComms() == Pstream::scheduled)
    {
        const lduSchedule& patchSchedule =
            bmesh_.mesh().globalData().patchSchedule();

        forAll(patchSchedule, patchEvali)
        {
            if (patchSchedule[patchEvali].init)
            {
                this->operator[](patchSchedule[patchEvali].patch)
                    .initEvaluate(Pstream::scheduled);
            }
            else
            {
                this->operator[](patchSchedule[patchEvali].patch)
                    .evaluate(Pstream::scheduled);
            }
        }
    }
    else
    {
        FatalErrorIn("GeometricBoundaryField::evaluate()")
            << "Unsuported communications type "
            << Pstream::commsTypeNames[Pstream::defaultCommsType()]
            << exit(FatalError);
    }
}


template<class Type, template<class> class PatchField, class GeoMesh>
void Foam::GeometricField<Type, PatchField, GeoMesh>::GeometricBoundaryField::
evaluateCoupled()
{
    if (debug)
    {
        Info<< "GeometricField<Type, PatchField, GeoMesh>::"
               "GeometricBoundaryField::"
               "evaluateCoupled()" << endl;
    }

    if
    (
        Pstream::defaultComms() == Pstream::blocking
     || Pstream::defaultComms() == Pstream::nonBlocking
    )
    {
        forAll(*this, patchi)
        {
            if (this->operator[](patchi).coupled())
            {
                this->operator[](patchi).initEvaluate
                (
                    Pstream::defaultComms()
                );
            }
        }

        // Block for any outstanding requests
        if (Pstream::defaultComms() == Pstream::nonBlocking)
        {
            IPstream::waitRequests();
            OPstream::waitRequests();
        }

        forAll(*this, patchi)
        {
            if (this->operator[](patchi).coupled())
            {
                this->operator[](patchi).evaluate
                (
                    Pstream::defaultComms()
                );
            }
        }
    }
    else if (Pstream::defaultComms() == Pstream::scheduled)
    {
        const lduSchedule& patchSchedule =
            bmesh_.mesh().globalData().patchSchedule();

        forAll(patchSchedule, patchEvali)
        {
            if (patchSchedule[patchEvali].init)
            {
                if
                (
                    this->operator[](patchSchedule[patchEvali].patch).coupled()
                )
                {
                    this->operator[](patchSchedule[patchEvali].patch)
                        .initEvaluate(Pstream::scheduled);
                }
            }
            else
            {
                if
                (
                    this->operator[](patchSchedule[patchEvali].patch).coupled()
                )
                {
                    this->operator[](patchSchedule[patchEvali].patch)
                        .evaluate(Pstream::scheduled);
                }
            }
        }
    }
    else
    {
        FatalErrorIn("GeometricBoundaryField::evaluateCoupled()")
            << "Unsuported communications type "
            << Pstream::commsTypeNames[Pstream::defaultCommsType()]
            << exit(FatalError);
    }
}


template<class Type, template<class> class PatchField, class GeoMesh>
Foam::wordList
Foam::GeometricField<Type, PatchField, GeoMesh>::GeometricBoundaryField::
types() const
{
    const FieldField<PatchField, Type>& pff = *this;

    wordList Types(pff.size());

    forAll(pff, patchi)
    {
        Types[patchi] = pff[patchi].type();
    }

    return Types;
}


template<class Type, template<class> class PatchField, class GeoMesh>
typename
Foam::GeometricField<Type, PatchField, GeoMesh>::GeometricBoundaryField
Foam::GeometricField<Type, PatchField, GeoMesh>::GeometricBoundaryField::
boundaryInternalField() const
{
    typename GeometricField<Type, PatchField, GeoMesh>::GeometricBoundaryField
        BoundaryInternalField(*this);

    forAll(BoundaryInternalField, patchi)
    {
        BoundaryInternalField[patchi] ==
            this->operator[](patchi).patchInternalField();
    }

    return BoundaryInternalField;
}


template<class Type, template<class> class PatchField, class GeoMesh>
Foam::lduInterfaceFieldPtrsList
Foam::GeometricField<Type, PatchField, GeoMesh>::GeometricBoundaryField::
interfaces() const
{
    lduInterfaceFieldPtrsList interfaces(this->size());

    forAll (interfaces, patchi)
    {
        if (isA<lduInterfaceField>(this->operator[](patchi)))
        {
            interfaces.set
            (
                patchi,
                &refCast<const lduInterfaceField>(this->operator[](patchi))
            );
        }
    }

    return interfaces;
}


template<class Type, template<class> class PatchField, class GeoMesh>
typename Foam::BlockLduInterfaceFieldPtrsList<Type>::Type
Foam::GeometricField<Type, PatchField, GeoMesh>::GeometricBoundaryField::
blockInterfaces() const
{
    typename BlockLduInterfaceFieldPtrsList<Type>::Type interfaces
    (
        this->size()
    );

    forAll (interfaces, patchi)
    {
        if (isA<BlockLduInterfaceField<Type> >(this->operator[](patchi)))
        {
            interfaces.set
            (
                patchi,
                &refCast<const BlockLduInterfaceField<Type> >
                (
                    this->operator[](patchi)
                )
            );
        }
    }

    return interfaces;
}


template<class Type, template<class> class PatchField, class GeoMesh>
void Foam::GeometricField<Type, PatchField, GeoMesh>::GeometricBoundaryField::
writeEntry(const word& keyword, Ostream& os) const
{
    // os  << keyword << nl << token::BEGIN_BLOCK << incrIndent << nl;
    os  << incrBlock(keyword);

    forAll(*this, patchi)
    {
        os  << incrBlock(this->operator[](patchi).patch().name())
            << this->operator[](patchi)
            << decrBlock << endl;
    }

    os << decrBlock << endl;
    // os  << decrIndent << token::END_BLOCK << endl;

    // Check state of IOstream
    os.check
    (
        "GeometricField<Type, PatchField, GeoMesh>::GeometricBoundaryField::"
        "writeEntry(const word& keyword, Ostream& os) const"
    );
}


// * * * * * * * * * * * * * * * Member Operators  * * * * * * * * * * * * * //

template<class Type, template<class> class PatchField, class GeoMesh>
void Foam::GeometricField<Type, PatchField, GeoMesh>::GeometricBoundaryField::
operator=
(
    const typename GeometricField<Type, PatchField, GeoMesh>::
    GeometricBoundaryField& bf
)
{
    FieldField<PatchField, Type>::operator=(bf);
}


template<class Type, template<class> class PatchField, class GeoMesh>
void Foam::GeometricField<Type, PatchField, GeoMesh>::GeometricBoundaryField::
operator=
(
    const FieldField<PatchField, Type>& ptff
)
{
    FieldField<PatchField, Type>::operator=(ptff);
}


template<class Type, template<class> class PatchField, class GeoMesh>
void Foam::GeometricField<Type, PatchField, GeoMesh>::GeometricBoundaryField::
operator=
(
    const Type& t
)
{
    FieldField<PatchField, Type>::operator=(t);
}


// Forced assignments
template<class Type, template<class> class PatchField, class GeoMesh>
void Foam::GeometricField<Type, PatchField, GeoMesh>::GeometricBoundaryField::
operator==
(
    const typename GeometricField<Type, PatchField, GeoMesh>::
    GeometricBoundaryField& bf
)
{
    forAll((*this), patchI)
    {
        this->operator[](patchI) == bf[patchI];
    }
}


template<class Type, template<class> class PatchField, class GeoMesh>
void Foam::GeometricField<Type, PatchField, GeoMesh>::GeometricBoundaryField::
operator==
(
    const FieldField<PatchField, Type>& ptff
)
{
    forAll((*this), patchI)
    {
        this->operator[](patchI) == ptff[patchI];
    }
}


template<class Type, template<class> class PatchField, class GeoMesh>
void Foam::GeometricField<Type, PatchField, GeoMesh>::GeometricBoundaryField::
operator==
(
    const Type& t
)
{
    forAll((*this), patchI)
    {
        this->operator[](patchI) == t;
    }
}


// * * * * * * * * * * * * * * * Friend Operators  * * * * * * * * * * * * * //

template<class Type, template<class> class PatchField, class GeoMesh>
Foam::Ostream& Foam::operator<<
(
    Ostream& os,
    const typename GeometricField<Type, PatchField, GeoMesh>::
    GeometricBoundaryField& bf
)
{
    os << static_cast<const FieldField<PatchField, Type>&>(bf);
    return os;
}


// ************************************************************************* //
