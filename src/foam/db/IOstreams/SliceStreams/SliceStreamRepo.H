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
    Foam::SliceStreamRepo

Description
    A repository for streaming resources for parallel I/O.

Author
    Gregor Weiss, HLRS University of Stuttgart, 2023
    Sergey Lesnik, Wikki GmbH, 2023
    Henrik Rusche, Wikki GmbH, 2023

SourceFiles
    SliceStreamRepoI.H
    SliceStreamRepo.C

\*---------------------------------------------------------------------------*/

#ifndef SliceStreamRepo_H
#define SliceStreamRepo_H

#include "label.H"

#include <map>
#include <memory>

// Forward declaration
namespace adios2
{
class ADIOS;
class IO;
class Engine;
}

namespace Foam
{

// Forward declaration
class string;


class SliceStreamRepo
{
    // Singelton instance
    static SliceStreamRepo* repoInstance_;

    // Private default constructor in singelton
    SliceStreamRepo();

    using IO_map = std::map<Foam::string, std::shared_ptr<adios2::IO>>;
    using Engine_map = std::map<Foam::string, std::shared_ptr<adios2::Engine>>;

    // Private members

    // Forward declaration of bridge to ADIOS2 dependencies
    class Impl;

    // Bridge instance to ADIOS2 implementation
    std::unique_ptr<Impl> pimpl_;

    label boundaryCounter_{0};

    // Private methods
    IO_map* get(const std::shared_ptr<adios2::IO>&);

    Engine_map* get(const std::shared_ptr<adios2::Engine>&);

public:

    // Getter to singelton instance
    static SliceStreamRepo* instance();

    // Destructor
    ~SliceStreamRepo();

    // Deleted copy constructor
    SliceStreamRepo(SliceStreamRepo& other) = delete;

    // Deleted copy assignment operator
    SliceStreamRepo& operator=(const SliceStreamRepo& other) = delete;

    // Getter for the ADIOS instance
    adios2::ADIOS* pullADIOS();

    // Pull of an ADIOS specific feature (IO, Engine, SliceBuffer)
    template<typename FeatureType>
    void pull(std::shared_ptr<FeatureType>&, const Foam::string&);

    // Push of an ADIOS specific feature (IO, Engine, SliceBuffer)
    template<typename FeatureType>
    void push(const std::shared_ptr<FeatureType>&, const Foam::string&);

    // Setter to boundaryCounter_
    // TODO: Not the responsibility of SliceStreamRepo. Can this be removed?
    void push(const label&);

    // Removal of an ADIOS specific feature (IO, Engine, SliceBuffer)
    template<typename FeatureType>
    void remove(const std::shared_ptr<FeatureType>&, const Foam::string&);

    // Initiating engines with Engine::BeginStep
    void open(const bool atScale = false);

    // Closing all engines and clear the engine map
    void close(const bool atScale = false);

    void clear();

};

}

#include "SliceStreamRepoI.H"

#endif

