#ifndef adiosRepo_H
#define adiosRepo_H

#include "label.H"

#include <map>
#include <memory>

namespace adios2 {
    class ADIOS;
    class IO;
    class Engine;
}

namespace Foam
{

// Forward declaration
class string;

class adiosRepo {

    // Singelton instance
    static adiosRepo* repoInstance_;

    // Private default constructor in singelton
    adiosRepo();

    using IO_map = std::map<Foam::string, std::shared_ptr<adios2::IO> >;
    using Engine_map = std::map<Foam::string, std::shared_ptr<adios2::Engine> >;

    // Private members

    // Bridge to ADIOS2 dependencies (PImple idiom)
    class Impl;
    std::unique_ptr<Impl> pimpl_{ nullptr };

    label boundaryCounter_{ 0 };

    // Private methods
    IO_map* get( const std::shared_ptr<adios2::IO>& );
    Engine_map* get( const std::shared_ptr<adios2::Engine>& );

  public:

    static adiosRepo* instance();

    ~adiosRepo();

    adiosRepo(adiosRepo& other) = delete;

    adiosRepo& operator=( const adiosRepo& other ) = delete;

    adios2::ADIOS* pullADIOS();

    template <typename FeatureType>
    void pull( std::shared_ptr<FeatureType>&, const Foam::string& );

    template <typename FeatureType>
    void push( const std::shared_ptr<FeatureType>&, const Foam::string& );

    void push( const label& );

    template <typename FeatureType>
    void remove( const std::shared_ptr<FeatureType>&, const Foam::string& );

    void close();

    void clear();

};

}


template <typename FeatureType>
void Foam::adiosRepo::pull( std::shared_ptr<FeatureType>& featurePtr,
                            const Foam::string& id ) {
    auto featureMap = get( featurePtr );
    if ( featureMap->count( id ) ) {
        featurePtr = featureMap->at( id );
    }
}

template <typename FeatureType>
void Foam::adiosRepo::push( const std::shared_ptr<FeatureType>& featurePtr,
                            const Foam::string& id ) {
    auto featureMap = get( featurePtr );
    if ( featureMap->count( id ) == 0 ) {
        featureMap->insert( { id, featurePtr } );
    }
}

template <typename FeatureType>
void Foam::adiosRepo::remove( const std::shared_ptr<FeatureType>& featurePtr,
                              const Foam::string& id ) {
    auto featureMap = get( featurePtr );
    if ( featureMap->count( id ) == 0 ) {
        featureMap->erase( id );
    }
}

#endif

