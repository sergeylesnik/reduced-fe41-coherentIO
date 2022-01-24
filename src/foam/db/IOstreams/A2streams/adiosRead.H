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

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                       Class adiosRead Declaration
\*---------------------------------------------------------------------------*/
class adiosRead
:
public adiosCore
{
    static std::unique_ptr<adios2::IO> ioReadPtr_;

    static std::unique_ptr<adios2::IO> ioReadMeshPtr_;

    static std::unique_ptr<adios2::Engine> enginePtr_;

    static std::unique_ptr<adios2::Engine> engineMeshPtr_;

    static const Foam::fileName meshPathname_;

    static const Foam::fileName dataPathname_;

    static bool filesChecked_;

    static bool dataPresent_;

    static bool meshPresent_;

    fileName pathname_;

    std::unique_ptr<adios2::Variable<double> > variablePtr_;

public:

    //- Constructor
    adiosRead(const fileName& pathname);

    //- Destructor
    ~adiosRead();

    // Access

        static const fileName& meshPathname();

        static void checkFiles();

        static bool dataPresent();

        static bool meshPresent();

        fileName pathname();

        std::unique_ptr<adios2::IO>& ioReadPtr();

        std::unique_ptr<adios2::IO>& ioReadMeshPtr();

        std::unique_ptr<adios2::Engine>& enginePtr();

        std::unique_ptr<adios2::Engine>& engineMeshPtr();

    void open();

    void defineVariable(const string name);

    void get(double* buf);

    void performGets();

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

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
