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
#include "ITstream.H"

static Foam::ITstream dummyITstream_("dummy", Foam::UList<Foam::token>());


// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //

Foam::scalarList Foam::fieldDataEntry::firstElement() const
{
    if (uListProxyPtr_->size())
    {
        label nCmpts = uListProxyPtr_->nComponents();

        scalarList L(uListProxyPtr_->nComponents());

        for(label i = 0; i < nCmpts; i++)
        {
            L[i] = uListProxyPtr_->cdata()[i];
        }

        return L;
    }

    return scalarList(0);
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::fieldDataEntry::fieldDataEntry
(
    const keyType& keyword,
    const word& compoundTokenName,
    uListProxyBase* uListProxyPtr
)
:
    entry(keyword),
    name_(),
    compoundTokenName_(compoundTokenName),
    uListProxyPtr_(uListProxyPtr),
    nGlobalElems_(0),
    tag_(uListProxyPtr->determineUniformity(), firstElement())
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

    return dummyITstream_;
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

    if (tag_.uniformityState() == uListProxyBase::UNIFORM)
    {
        os  << "uniform" << token::SPACE;

        // If EMPTY, data is nullptr. Thus, use the field tag to provide the
        // first element.
        uListProxyPtr_->writeFirstElement
        (
            os,
            tag_.firstElement().cdata()
        );
    }
    else
    {
        os  << "nonuniform" << token::SPACE << compoundTokenName_
            << token::SPACE << nGlobalElems_ << fn;
    }

    os << token::END_STATEMENT << endl;
}


// ************************************************************************* //
