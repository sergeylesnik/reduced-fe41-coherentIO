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

#include "UList.H"
#include "Ostream.H"
#include "token.H"
#include "SLList.H"
#include "contiguous.H"
#include <iostream>

#include "prefixOSstream.H"

#include "UListProxy.H"

// * * * * * * * * * * * * * * * Ostream Operator *  * * * * * * * * * * * * //

namespace Foam {
    extern prefixOSstream Pout;
}

template<class T>
void Foam::UList<T>::writeEntry(Ostream& os) const
{
    if (debug)
    {
        Pout<< "UList<T>::writeEntry(Ostream&): List< ... >" << endl;
    }
    if
    (
        size()
     && token::compound::isCompound
        (
            "List<" + word(pTraits<T>::typeName) + '>'
        )
    )
    {
        os  << word("List<" + word(pTraits<T>::typeName) + '>') << " ";
    }

    os << *this;
}


template<class T>
void Foam::UList<T>::writeEntry(const word& keyword, Ostream& os) const
{
    if (debug)
    {
        Pout<< "UListIO::writeEntry keyword = " << keyword << endl;
    }

    os.writeKeyword(keyword);
    writeEntry(os);
    os << token::END_STATEMENT << endl;
}


template<class T>
Foam::Ostream& Foam::operator<<(Foam::Ostream& os, const Foam::UList<T>& L)
{
    // Do not print this debug info if a UList is printed to the screen
    // by testing whether the stream is prefixOSstream (e.g. Pout)
    if (UList<T>::debug && !isA<class prefixOSstream>(os))
    {
        Pout<< "UListIO: operator<<(); id = " << os.getBlockId() << endl;
    }

    // Write list contents depending on data format
    if (os.format() == IOstream::ASCII || !contiguous<T>())
    {
        if (UList<T>::debug && !isA<class prefixOSstream>(os))
        {
            Pout<< "UListIO: writing ASCII because";
            if (!contiguous<T>())
            {
                Pout<< " the template parameter T is non-contiguous" << endl;
            }
            else
            {
                Pout<< " streamFormat is ASCII" << endl;
            }
        }

        bool uniform = false;

        if (L.size() > 1 && contiguous<T>())
        {
            uniform = true;

            forAll(L, i)
            {
                if (L[i] != L[0])
                {
                    uniform = false;
                    break;
                }
            }
        }

        if (uniform)
        {
            // Write size and start delimiter
            os << L.size() << token::BEGIN_BLOCK;

            // Write contents
            os << L[0];

            // Write end delimiter
            os << token::END_BLOCK;
        }
        else if (L.size() <= 1 || (L.size() < 11 && contiguous<T>()))
        {
            // Write size and start delimiter
            os << L.size() << token::BEGIN_LIST;

            // Write contents
            forAll(L, i)
            {
                if (i > 0) os << token::SPACE;
                os << L[i];
            }

            // Write end delimiter
            os << token::END_LIST;
        }
        else if (os.format() == IOstream::COHERENT) // is also non-contiguous
        {
            if (UList<T>::debug && !isA<class prefixOSstream>(os))
            {
                Pout<< "UListIO: writing non-contiguous data as string via"
                    << " COHERENT" << endl;
            }

            // Write size and identifier
            os  << nl << L.size() << os.getBlockId() << nl;

            // Write contents to a stringStream which is written by ADIOS
            // at destruction of os
            Ostream& oss = os.stringStream();
            forAll(L, i)
            {
                oss << nl << L[i];
            }
        }
        else
        {
            // Write size and start delimiter
            os << nl << L.size() << nl << token::BEGIN_LIST;

            // Write contents
            forAll(L, i)
            {
                os << nl << L[i];
            }

            // Write end delimiter
            os << nl << token::END_LIST << nl;
        }
    }
    else if (os.format() == IOstream::BINARY)
    {
        os << nl << L.size() << nl;
        if (L.size())
        {
            os.write(reinterpret_cast<const char*>(L.v_), L.byteSize());
        }
    }
    else if (os.format() == IOstream::COHERENT)
    {
        if(UList<T>::debug)
        {
            const string id = os.getBlockId();
            Pout<< "Writing a field of size " << L.byteSize()
                << " via COHERENT IO with identifier:\n    "
                << id << endl;
            Pout<< "L = " << L << endl;
        }

        std::unique_ptr<UListProxy<T>> proxy(new UListProxy<T>(L));
        os.parwrite(std::move(proxy));
        //assert(proxy == nullptr);
    }

    // Check state of IOstream
    os.check("Ostream& operator<<(Ostream&, const UList&)");

    return os;
}


template<class T>
Foam::Istream& Foam::operator>>(Istream& is, UList<T>& L)
{
    if (UList<T>::debug)
    {
        Pout<< "UListIO: in operator>>(Istream& is, UList<T>& L), id = "
            << endl;
    }

    is.fatalCheck("operator>>(Istream&, UList<T>&)");

    token firstToken(is);

    is.fatalCheck("operator>>(Istream&, UList<T>&) : reading first token");

    if (firstToken.isCompound())
    {
        List<T> elems;
        elems.transfer
        (
            dynamicCast<token::Compound<List<T> > >
            (
                firstToken.transferCompoundToken(is)
            )
        );
        // Check list length
        label s = elems.size();

        if (s != L.size())
        {
            FatalIOErrorIn("operator>>(Istream&, UList<T>&)", is)
                << "incorrect length for UList. Read " << s
                << " expected " << L.size()
                << exit(FatalIOError);
        }
        for (label i=0; i<s; i++)
        {
            L[i] = elems[i];
        }
    }
    else if (firstToken.isLabel())
    {
        label s = firstToken.labelToken();

        // Set list length to that read
        if (s != L.size())
        {
            FatalIOErrorIn("operator>>(Istream&, UList<T>&)", is)
                << "incorrect length for UList. Read " << s
                << " expected " << L.size()
                << exit(FatalIOError);
        }

        // Read list contents depending on data format

        if (is.format() == IOstream::ASCII || !contiguous<T>())
        {
            if(UList<T>::debug)
            {
                Pout<< "Writing a field as ASCII of size "
                    << L.byteSize() << endl;
            }

            // Read beginning of contents
            char delimiter = is.readBeginList("List");

            if (s)
            {
                if (delimiter == token::BEGIN_LIST)
                {
                    for (label i=0; i<s; i++)
                    {
                        is >> L[i];

                        is.fatalCheck
                        (
                            "operator>>(Istream&, UList<T>&) : reading entry"
                        );
                    }
                }
                else
                {
                    T element;
                    is >> element;

                    is.fatalCheck
                    (
                        "operator>>(Istream&, UList<T>&) : "
                        "reading the single entry"
                    );

                    for (label i=0; i<s; i++)
                    {
                        L[i] = element;
                    }
                }
            }

            // Read end of contents
            is.readEndList("List");
        }
        else if (is.format() == IOstream::BINARY)
        {
            if (s)
            {
                is.read(reinterpret_cast<char*>(L.data()), s*sizeof(T));

                is.fatalCheck
                (
                    "operator>>(Istream&, UList<T>&) : reading the binary block"
                );
            }
        }
        else if (is.format() == IOstream::COHERENT)
        { cout << "Parallel IO not yet implemented in UListIO.C\n"; }
    }
    else if (firstToken.isPunctuation())
    {
        if (firstToken.pToken() != token::BEGIN_LIST)
        {
            FatalIOErrorIn("operator>>(Istream&, UList<T>&)", is)
                << "incorrect first token, expected '(', found "
                << firstToken.info()
                << exit(FatalIOError);
        }

        // Putback the opening bracket
        is.putBack(firstToken);

        // Now read as a singly-linked list
        SLList<T> sll(is);

        if (sll.size() != L.size())
        {
            FatalIOErrorIn("operator>>(Istream&, UList<T>&)", is)
                << "incorrect length for UList. Read " << sll.size()
                << " expected " << L.size()
                << exit(FatalIOError);
        }

        // Convert the singly-linked list to this list
        label i = 0;
        for
        (
            typename SLList<T>::const_iterator iter = sll.begin();
            iter != sll.end();
            ++iter
        )
        {
            L[i] = iter();
        }

    }
    else
    {
        FatalIOErrorIn("operator>>(Istream&, UList<T>&)", is)
            << "incorrect first token, expected <int> or '(', found "
            << firstToken.info()
            << exit(FatalIOError);
    }

    return is;
}


// ************************************************************************* //
