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

#ifndef adiosCore_H
#define adiosCore_H

#include "adios2.h"
#include "Engine.h"

#include "word.H"
#include "labelList.H"
#include <memory>
#include <map>

#include "adiosVariableData.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                       Class adiosCore Declaration
\*---------------------------------------------------------------------------*/
class adiosCore
{

    //- ADIOS instance
    static std::unique_ptr<adios2::ADIOS> adiosPtr_;

    static std::unique_ptr<adios2::IO> ioWritePtr_;

    static std::unique_ptr<adios2::Engine> enginePtr_;

    static std::unique_ptr< adiosVariableData > variableDataPtr_;

    static Foam::fileName pathname_;

    static const Foam::fileName meshPathname_;

    static const Foam::fileName dataPathname_;

    static bool filesChecked_;

    static bool dataPresent_;

    static bool meshPresent_;


public:

    // Declare name of the class and its debug switch
    ClassName("adiosCore");


    //- Constructors
    adiosCore() = default;

    //- Disallow copy constructor
    adiosCore(const adiosCore&) = delete;

    //- Destructor
    ~adiosCore() = default;

    // Access

        //- Set path to the file
        static void setPathName( const fileName& pathname ) {
            pathname_ = pathname;
        }

        static void beginStep();

        static void endStep();

        static void close();

        static const fileName& meshPathname();

        static const fileName& dataPathname();

        static void checkFiles();

        static bool dataPresent();

        static bool meshPresent();

    //- Disallow copy assignment
    adiosCore& operator=(const adiosCore&) = delete;

    // Access

        //- ADIOS instance
        static std::unique_ptr<adios2::ADIOS>& adiosPtr();

        static std::unique_ptr<adios2::IO>& ioWritePtr();

        static std::unique_ptr<adios2::Engine>& enginePtr();

    template< typename T >
    static void makeVariable( const string name,
                              const label shape,
                              const label start,
                              const label count );

    template< typename T >
    static void makeVariableWithSpan( const string name,
                                      const label shape,
                                      const label start,
                                      const label count );

    template< typename T >
    static void write( const Foam::string blockId,
                       const T* buf );
};

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //


template< typename T >
void Foam::adiosCore::makeVariable
(
    const Foam::string name,
    const Foam::label shape,
    const Foam::label start,
    const Foam::label count
)
{
    variableDataPtr_ = adiosVariableData::makeVariable<T>
                           (
                               ioWritePtr().get(),
                               name,
                               count
                           );
}


template< typename T >
void Foam::adiosCore::makeVariableWithSpan( const Foam::string name,
                                            const Foam::label shape,
                                            const Foam::label start,
                                            const Foam::label count ) {
    variableDataPtr_ = adiosVariableData::makeVariableWithSpan<T>
                           (
                               ioWritePtr().get(),
                               enginePtr().get(),
                               name,
                               count
                           );

    variableDataPtr_->createSpan( adiosCore::enginePtr().get() );
}

template< typename T >
void Foam::adiosCore::write( const Foam::string blockId, const T* buf ) {
    enginePtr()->Put( static_cast<const adiosVariable<T>*>( variableDataPtr_.get() )->variable_, buf, adios2::Mode::Sync );
}


#endif

// ************************************************************************* //
