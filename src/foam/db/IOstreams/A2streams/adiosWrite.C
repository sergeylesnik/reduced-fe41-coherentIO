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

#include "OSspecific.H"
#include "adiosWrite.H"


// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

std::unique_ptr<adios2::IO> Foam::adiosWrite::ioWritePtr_ = nullptr;

std::unique_ptr<adios2::Engine> Foam::adiosWrite::enginePtr_ = nullptr;

Foam::fileName Foam::adiosWrite::pathname_ =
    Foam::getEnv("FOAM_CASE")/"data.bp";


// * * * * * * * * * * * * Static Member Functions * * * * * * * * * * * * * //

void Foam::adiosWrite::beginStep()
{
    enginePtr()->BeginStep();
}


void Foam::adiosWrite::endStep()
{
    enginePtr()->EndStep();
}


void Foam::adiosWrite::close()
{
    if (enginePtr_)
    {
        enginePtr_->Close();
        enginePtr_.reset(nullptr);
    }
}

void Foam::adiosWrite::setPathName(const fileName& pathname)
{
    pathname_ = pathname;
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::adiosWrite::adiosWrite()
:
    variablePtr_(nullptr)
{}


// * * * * * * * * * * * * * * * * Destructors * * * * * * * * * * * * * * * //

Foam::adiosWrite::~adiosWrite()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

Foam::fileName Foam::adiosWrite::pathname()
{
    return this->pathname_;
}


std::unique_ptr<adios2::IO>& Foam::adiosWrite::ioWritePtr()
{
    if(!ioWritePtr_)
    {
        ioWritePtr_.reset
        (
            new adios2::IO(adiosPtr()->DeclareIO("write"))
        );
        ioWritePtr_->SetEngine("BP4");
        // Parameters to consider for increasing the IO performance
        // ioWritePtr_->SetParameter("MaxBufferSize", "100Kb");
        // ioWritePtr_->SetParameter("FlushStepsCount", "100");
    }

    return ioWritePtr_;
}


std::unique_ptr<adios2::Engine>& Foam::adiosWrite::enginePtr()
{
    if(!enginePtr_)
    {
        enginePtr_.reset
        (
            new adios2::Engine
            (
                ioWritePtr()->Open(pathname_, adios2::Mode::Write)
            )
        );
    }

    return enginePtr_;
}


void Foam::adiosWrite::defineVariable
(
    const Foam::string name,
    const Foam::label shape,
    const Foam::label start,
    const Foam::label count,
    const bool constantDims
)
{
    if (adiosCore::debug)
    {
        Pout<< "Define variable: " << name << endl;
    }

    variablePtr_.reset
    (
        new adios2::Variable<double>
        (
            ioWritePtr()->DefineVariable<double>
            (
                name,
                {},
                {},
                {static_cast<size_t>(count)},
                constantDims
            )
        )
    );
}


void Foam::adiosWrite::open()
{
    enginePtr();
}


void Foam::adiosWrite::put(const double* buf)
{
    if (adiosCore::debug)
    {
        Pout<< "adiosWrite::put(const char* buf)" << endl;
    }

    enginePtr()->Put(*variablePtr_, buf);
}


void Foam::adiosWrite::performPuts()
{
    if (adiosCore::debug)
    {
        Pout<< "adiosWrite::performPuts()" << endl;
    }

    enginePtr()->PerformPuts();
}


void Foam::adiosWrite::write
(
    const Foam::string blockId,
    const Foam::label shape,
    const Foam::label start,
    const Foam::label count,
    const parIOType* buf
)
{
    if (adiosCore::debug)
    {
        Pout<< "adiosWrite::write : writing a variable" << nl
            << "    with name = " << blockId << nl
            << "    and pathname = " << nl
            << "    " << pathname_ << endl;
    }

    // Use only the pointer infrastructure from this class by now for clarity
    adios2::Variable<parIOType> var =
        ioWritePtr()->DefineVariable<parIOType>
        (
            blockId,
            {},
            {},
            {static_cast<size_t>(count)},
            adios2::ConstantDims
        );

    // Step variant works in Mode::Append. Close() is called in parRunControl
    // before MPI_Finalize.
    // enginePtr()->BeginStep();
    enginePtr()->Put(var, buf);
    // enginePtr()->EndStep();
    // close();

    // PerformPuts variant.
    // ?adios bug? :
    //   Close() called here in Mode::Append : garbage data for ranks > 0
    //   Close() called only once in parRunControl: Independent of Mode, only
    //     the data from the last opened engine is present in the file.
    // enginePtr()->Put(var, buf);
    // enginePtr()->PerformPuts();
    // enginePtr()->Flush();
    // close();
}


void Foam::adiosWrite::writeLocalString
(
    const Foam::fileName& varName,
    const std::string& str,
    const label size
)
{
    adios2::Variable<std::string> var =
        ioWritePtr()->DefineVariable<std::string>
        (
            varName,
            {adios2::LocalValueDim}
        );

    // Step variant works in Mode::Append. Close() is called in parRunControl
    // before MPI_Finalize.
    // enginePtr()->BeginStep();
    enginePtr()->Put(var, str);
    // enginePtr()->EndStep();
    // close();
}

// ************************************************************************* //
