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
class adiosBuffer;

class adiosRepo {

    class Impl;
    static std::unique_ptr<Impl> pimpl_;

    std::map<Foam::string, std::shared_ptr<adios2::IO> >* get( const std::shared_ptr<adios2::IO>& );
    std::map<Foam::string, std::shared_ptr<adios2::Engine> >* get( const std::shared_ptr<adios2::Engine>& );
    std::map<Foam::string, std::shared_ptr<adiosBuffer> >* get( const std::shared_ptr<adiosBuffer>& );

    label boundaryCounter_{ 0 };

public:

    adiosRepo();
    ~adiosRepo();

    adios2::ADIOS* pullADIOS();

    template <typename FeatureType>
    void pull( std::shared_ptr<FeatureType>&, const Foam::string& );

    template <typename FeatureType>
    void push( const std::shared_ptr<FeatureType>&, const Foam::string& );

    void push( const label& );

    template <typename FeatureType>
    void remove( const std::shared_ptr<FeatureType>&, const Foam::string& );

    void close();

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

