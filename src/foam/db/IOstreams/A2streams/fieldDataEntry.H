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
    Foam::fieldDataEntry

Description
    Entry class to store field relevant data in a dictionary

SourceFiles
    fieldDataEntry.C

\*---------------------------------------------------------------------------*/

#ifndef fieldDataEntry_H
#define fieldDataEntry_H

#include "dictionary.H"
#include "fieldTag.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                          Class fieldDataEntry Declaration
\*---------------------------------------------------------------------------*/

class fieldDataEntry
:
    public entry
{

    // Private data

        //- Entry name
        fileName name_;

        //- Compound token associated with the data, i.e. List<...>
        const token compoundToken_;

        //- Pointer to the data
        const scalar* data_;

        //- Size of the complete data in bytes
        const label byteSize_;

        //- Number of elements in the data list
        const label nElements_;

        //- Number of components in each element
        const label nCmpts_;

        //- Uniformity property
        fieldTag::uniformity uniformity_;

        //- Number of elements summed over all ranks
        label nGlobalElements_;

    // Private member functions

        //- Determine uniformity of the data
        fieldTag::uniformity determineUniformity() const;

public:

    // Constructors

        fieldDataEntry
        (
            const keyType& keyword,
            const token& compoundToken,
            const scalar* data,
            std::streamsize byteSize,
            label nElements
        );

        //- Construct on freestore as copy with reference to the
        //  dictionary the copy belongs to
        virtual autoPtr<entry> clone(const dictionary&) const
        {
            return autoPtr<entry>(new fieldDataEntry(*this));
        }


    //-Destructor
    virtual ~fieldDataEntry()
    {}

    // Member functions

        //- Return the dictionary name
        virtual const fileName& name() const
        {
            return name_;
        }

        //- Return the dictionary name
        virtual fileName& name()
        {
            return name_;
        }

        //- Return line number of first token in dictionary
        virtual label startLineNumber() const;

        //- Return line number of last token in dictionary
        virtual label endLineNumber() const;

        //- Return true if this entry is a stream
        virtual bool isStream() const
        {
            return false;
        }

        //- This entry is not a primitive,
        //  calling this function generates a FatalError
        virtual ITstream& stream() const;

        //- This entry is not a dictionary,
        //  calling this function generates a FatalError
        virtual const dictionary& dict() const;

        //- This entry is not a dictionary,
        //  calling this function generates a FatalError
        virtual dictionary& dict();

        //- Write
        virtual void write(Ostream&) const;

        // Access

            //- Return number of bytes
            inline token compoundToken() const
            {
                return compoundToken_;
            }

            //- Return number of bytes
            inline label byteSize() const
            {
                return byteSize_;
            }

            //- Return number of the elements
            inline label nElements() const
            {
                return nElements_;
            }

            //- Return number of the field components
            inline label nCmpts() const
            {
                return nCmpts_;
            }

            inline fileName id() const
            {
                fileName id = name_;
                id.replaceAll("::", "/");

                return id;
            }

            inline const scalar* data() const
            {
                return data_;
            }

            inline bool uniform() const
            {
                if (uniformity_ == fieldTag::UNIFORM)
                {
                    return true;
                }

                return false;
            }

        //- Return first element of the data list
        scalarList firstElement() const;

        //- Return uniformity of the data list
        inline fieldTag::uniformity getUniformity() const
        {
            return uniformity_;
        }

        //- Return access to the uniformity of the data list
        inline fieldTag::uniformity& getUniformity()
        {
            return uniformity_;
        }

        //- Set number of elements summed over all ranks
        inline void setNGlobalElements(const label nElem)
        {
            nGlobalElements_ = nElem;
        }
};

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
