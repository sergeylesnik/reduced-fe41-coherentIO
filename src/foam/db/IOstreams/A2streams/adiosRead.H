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

#ifndef adiosRead_H
#define adiosRead_H

#include "IOstream.H"
#include "adiosCore.H"
#include "adiosVariableData.H"
#include "adios2.h"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

template<typename Container>
void readAdiosToContainer( const Foam::string blockId,
                           Container& container );


/*---------------------------------------------------------------------------*\
                       Class adiosRead Declaration
\*---------------------------------------------------------------------------*/
class adiosRead
{
    static adiosCore adiosCore_;

    static std::unique_ptr<adios2::IO> ioReadPtr_;

    static std::unique_ptr<adios2::IO> ioReadMeshPtr_;

    static std::unique_ptr<adios2::Engine> enginePtr_;

    static std::unique_ptr<adios2::Engine> engineMeshPtr_;

    static fileName pathname_;

    std::unique_ptr<adios2::Variable<double> > variablePtr_;

    static std::unique_ptr< adiosVariableData > variableDataPtr_;

public:

    //- Constructor
    adiosRead();

    //- Destructor
    ~adiosRead();

    // Access

        fileName pathname();

        static std::unique_ptr<adios2::IO>& ioReadPtr();

        static std::unique_ptr<adios2::IO>& ioReadMeshPtr();

        static std::unique_ptr<adios2::Engine>& enginePtr();

        static std::unique_ptr<adios2::Engine>& engineMeshPtr();

        static const fileName& meshPathname() {
            return adiosCore_.meshPathname();
        }

        static const fileName& dataPathname() {
            return adiosCore_.dataPathname();
        }

        static void checkFiles() {
            adiosCore_.checkFiles();
        }

        static bool dataPresent() {
            return adiosCore_.dataPresent();
        }

        static bool meshPresent() {
            return adiosCore_.meshPresent();
        }

        static std::unique_ptr<adios2::ADIOS>& adiosPtr_() {
            return adiosCore_.adiosPtr();
        }

    void open();

    void defineVariable(const string name);

    void get(double* buf);

    void performGets();

    template< typename T >
    static label getVariable( const string name );

    template< typename T >
    static void read( const string name,
                      T* buf );

    void read
    (
        parIOType* buf,
        const string name
    );

    bool readLocalString
    (
        std::string& buf,
        const Foam::string strName
    );
};

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

template< typename T >
Foam::label Foam::adiosRead::getVariable( const Foam::string name )
{
    variableDataPtr_ = adiosVariableData::getVariable<T>
                           (
                               ioReadMeshPtr().get(),
                               name
                           );

    auto var = static_cast<const adiosVariable<T>*>( variableDataPtr_.get() )->variable_;
    Foam::label count{};
    if ( static_cast<const adiosVariable<T>*>( variableDataPtr_.get() )->variable_ )
    {
        auto blocksInfo = engineMeshPtr()->BlocksInfo( static_cast<const adiosVariable<T>*>( variableDataPtr_.get() )->variable_, 0 );
        count = blocksInfo[0].Count[0];
    }

    return count;
}

template<typename T>
void Foam::adiosRead::read
(
    const Foam::string blockId,
    T* buf
)
{
    engineMeshPtr()->Get( static_cast<const adiosVariable<T>*>( variableDataPtr_.get() )->variable_, buf, adios2::Mode::Sync );
}

template<typename Container>
void Foam::readAdiosToContainer
(
    const Foam::string blockId,
    Container& container
)
{
    typedef typename Container::value_type ElementType;
    Foam::label count = Foam::adiosRead::getVariable<ElementType>( blockId );
    container.resize( count );
    Foam::adiosRead::read( blockId, container.data() );
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
