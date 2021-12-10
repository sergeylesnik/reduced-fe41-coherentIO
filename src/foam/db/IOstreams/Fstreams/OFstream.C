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

#include "OFstream.H"
#include "OSspecific.H"
#include "UList.H"
#include "Pstream.H"
#include "gzstream.h"
#include "messageStream.H"
#include <iostream>
#include <memory>

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

defineTypeNameAndDebug(Foam::OFstream, 0);


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

Foam::OFstreamAllocator::OFstreamAllocator
(
    const fileName& pathname,
    ios_base::openmode mode,
    IOstream::streamFormat format,
    IOstream::compressionType compression
)
:
    ofPtr_(nullptr),
    adiosPtr_(nullptr)
{
    if (pathname.empty())
    {
        if (OFstream::debug)
        {
            Info<< "OFstreamAllocator::OFstreamAllocator(const fileName&) : "
                   "cannot open null file " << endl;
        }
    }

    if (compression == IOstream::COMPRESSED)
    {
        // get identically named uncompressed version out of the way
        if (isFile(pathname, false))
        {
            rm(pathname);
        }

        ofPtr_ = new ogzstream((pathname + ".gz").c_str(), mode);
    }
    else
    {
        // get identically named compressed version out of the way
        if (isFile(pathname + ".gz", false))
        {
            rm(pathname + ".gz");
        }

        ofPtr_ = new ofstream(pathname.c_str(), mode);

        // create an extra file with extension .dat to split off the field values
        if (format == IOstream::PARALLEL)
        {
            fileName parpathname = pathname.path();

            // Escape uniform folder if applicable
            if (parpathname.name().find("uniform") != std::string::npos)
            { parpathname = parpathname + "/../"; }

            // Escape time folder
            parpathname = parpathname + "/../";
            parpathname.clean();

            // Escape processor folder if applicable
            if (parpathname.name().find("processor") != std::string::npos)
            { parpathname = parpathname + "/../"; }

            parpathname = parpathname + "/data.bp";
            parpathname.clean();

            adiosPtr_.reset(new adiosWrite(parpathname));
        }
    }
}


Foam::OFstreamAllocator::~OFstreamAllocator()
{
    delete ofPtr_;
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::OFstream::OFstream
(
    const fileName& pathname,
    ios_base::openmode mode,
    streamFormat format,
    versionNumber version,
    compressionType compression
)
:
    OFstreamAllocator(pathname, mode, format, compression),
    OSstream(*ofPtr_, adiosPtr_, "OFstream.sinkFile_", format, version, compression),
    pathname_(pathname),
    blockNamesStack_()
{
    setClosed();
    setState(ofPtr_->rdstate());

    if (!good())
    {
        if (debug)
        {
            Info<< "IFstream::IFstream(const fileName&,"
                   "streamFormat format=ASCII,"
                   "versionNumber version=currentVersion) : "
                   "could not open file for input\n"
                   "in stream " << info() << Foam::endl;
        }

        setBad();
    }
    else
    {
        setOpened();
    }

    lineNumber_ = 1;
}


// * * * * * * * * * * * * * * * * Destructors * * * * * * * * * * * * * * * //

Foam::OFstream::~OFstream()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

Foam::word Foam::OFstream::getBlockId()
{
    word id = "";
    bool firstEntry = true;
    id = word(blockNamesStack_.size());

    forAllConstIter(SLList<word>, blockNamesStack_, iter)
    {
        if (firstEntry)
        {
            id = iter();
            firstEntry = false;
        }
        else
        {
            id = iter() + '/' + id;
        }
    }

    id = pathname_.caseName("") + '/' + id;


    if(debug > 1)
    {
        Pout
            << "Block id = " << id << "; blockNamesStack_.size() = "
            << blockNamesStack_.size() << '\n';
    }

    return id;
}


std::ostream& Foam::OFstream::stdStream()
{
    if (!ofPtr_)
    {
        FatalErrorIn("OFstream::stdStream()")
            << "No stream allocated." << abort(FatalError);
    }
    return *ofPtr_;
}


const std::ostream& Foam::OFstream::stdStream() const
{
    if (!ofPtr_)
    {
        FatalErrorIn("OFstreamAllocator::stdStream() const")
            << "No stream allocated." << abort(FatalError);
    }
    return *ofPtr_;
}


void Foam::OFstream::print(Ostream& os) const
{
    os  << "    OFstream: ";
    OSstream::print(os);
}


Foam::word Foam::OFstream::incrBlock(const word name)
{
    blockNamesStack_.push(name);

    if(debug)
    {
        Pout
            << "Add to the block name LIFO stack: " << name << '\n';
    }

    this->indent();
    this->write(name);
    this->write(nl);
    this->indent();
    this->write(char(token::BEGIN_BLOCK));
    this->write(nl);
    this->incrIndent();

    return name;
}


void Foam::OFstream::decrBlock()
{
    popBlockNamesStack();

    this->decrIndent();
    this->indent();
    this->write(char(token::END_BLOCK));
}


Foam::Ostream& Foam::OFstream::writeKeyword(const keyType& kw)
{
    if (debug)
    {
        Pout
            << "Add to the block name LIFO stack: " << kw << "\n";
    }

    blockNamesStack_.push(kw);

    return this->Ostream::writeKeyword(kw);
}


void Foam::OFstream::popBlockNamesStack()
{
    if (blockNamesStack_.empty())
    {
        WarningInFunction
            << "Tried to pop the stack although its empty.\n";
    }
    else
    {
        if(debug)
        {
            Pout
                << "Pop block name LIFO stack: "
                << blockNamesStack_.first() << '\n';
        }

        blockNamesStack_.pop();
    }
}


Foam::Ostream& Foam::OFstream::parwrite(const char* buf, const label count)
{
    if (format() != PARALLEL)
    {
        FatalIOErrorIn("Ostream::parwrite(const char*, const label)", *this)
            << "stream format not parallel"
            << abort(FatalIOError);
    }

    word blockId = getBlockId();
    adiosPtr_->write(blockId, count, 0, count, buf);

    return *this;
}

// ************************************************************************* //
