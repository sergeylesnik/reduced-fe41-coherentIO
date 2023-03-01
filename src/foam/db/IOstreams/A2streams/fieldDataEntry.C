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

#include "fieldDataEntry.H"

// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //

Foam::fieldTag::uniformity
Foam::fieldDataEntry::determineUniformity() const
{
    if (nElements_ != 0)
    {
        fieldTag::uniformity u = fieldTag::UNIFORM;

        // Skip comparison of the first element with itself
        for (label i = 1; i < nElements_; i++)
        {
            const label offset = i*nCmpts_;

            for (label cmptI = 0; cmptI < nCmpts_; cmptI++)
            {
                if (data_[offset + cmptI] != data_[cmptI])
                {
                    u = fieldTag::NONUNIFORM;
                    break;
                }
            }
        }

        return u;
    }
    else
    {
        return fieldTag::EMPTY;
    }
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::fieldDataEntry::fieldDataEntry
(
    const keyType& keyword,
    const token& compoundToken,
    const scalar* data,
    std::streamsize byteSize,
    label nElements
)
:
    entry(keyword),
    name_(),
    compoundToken_(compoundToken),
    data_(data),
    byteSize_(byteSize),
    nElements_(nElements),
    nCmpts_(nElements ? (byteSize/nElements/sizeof(scalar)) : 0),
    uniformity_(determineUniformity()),
    nGlobalElements_(0)
{}


// * * * * * * * * * * * * * * * * Destructors * * * * * * * * * * * * * * * //


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

Foam::label Foam::fieldDataEntry::startLineNumber() const
{
    return 0;
}


Foam::label Foam::fieldDataEntry::endLineNumber() const
{
    return 0;
}


Foam::ITstream& Foam::fieldDataEntry::stream() const
{
    FatalErrorInFunction
        << "Attempt to return field data entry " << keyword()
        << " as a primitive"
        << abort(FatalError);

    ITstream dummyStream("", UList<token>());
    return dummyStream;
}


const Foam::dictionary& Foam::fieldDataEntry::dict() const
{
    FatalErrorInFunction
        << "Attempt to return primitive entry " << keyword()
        << " as a sub-dictionary"
        << abort(FatalError);

    return dictionary::null;
}


Foam::dictionary& Foam::fieldDataEntry::dict()
{
    FatalErrorInFunction
        << "Attempt to return primitive entry " << keyword()
        << " as a sub-dictionary"
        << abort(FatalError);

    return const_cast<dictionary&>(dictionary::null);
}


void Foam::fieldDataEntry::write(Ostream& os) const
{
    // dict uses "::" as a separator - replace it with the fileName standard "/"
    fileName fn = name();
    fn.replaceAll("::", "/");
    os.writeKeyword(fn.name());

    if (uniformity_ == fieldTag::UNIFORM)
    {
        os  << "uniform" << token::SPACE;

        if (nCmpts_ > 1)
        {
            // Mimic VectorSpace write
            os  << token::BEGIN_LIST;

            for (label i = 0; i < (nCmpts_ - 1); i++)
            {
                os << data_[i] << token::SPACE;
            }

            os << data_[nCmpts_ - 1] << token::END_LIST;
        }
        else // single scalar
        {
            os  << data_[0];
        }
    }
    else
    {
        os  << "nonuniform" << token::SPACE << compoundToken_ << token::SPACE
            << nGlobalElements_ << fn;
    }

    os << token::END_STATEMENT << endl;
}


Foam::scalarList Foam::fieldDataEntry::firstElement() const
{
    scalarList L(nCmpts_);
    for(label i = 0; i < nCmpts_; i++)
    {
        L[i] = data_[i];
    }

    return L;
}


// ************************************************************************* //
