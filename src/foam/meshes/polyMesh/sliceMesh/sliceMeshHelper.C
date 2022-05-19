
#include "sliceMeshHelper.H"


std::vector<std::pair<Foam::label, Foam::label> >
Foam::generateIndexedPairs( const std::vector<Foam::label>& input ) {
    std::vector<Foam::label> indices{};
    indexIota( indices, input.size(), 0 );
    std::vector<std::pair<Foam::label, Foam::label> > indexedPairs{};
    indexedPairs.reserve( input.size() );
    std::transform( input.begin(),
                    input.end(),
                    indices.begin(),
                    std::back_inserter( indexedPairs ),
                    []( const auto& inputId, const auto& index )
                    { return std::make_pair( inputId, index ); } );
    return indexedPairs;
}


void Foam::partitionByFirst( std::vector<std::pair<Foam::label, Foam::label> >& input ) {
    std::stable_partition( input.begin(),
                           input.end(),
                           [](const auto& n)
                           { return n.first>0; } );
    std::stable_sort( std::partition_point( input.begin(),
                                            input.end(),
                                            [](const auto& n)
                                            { return n.first>0; } ),
                      input.end(),
                      [](const auto& n, const auto& m)
                      { return n.first>m.first; } );
}


void Foam::renumberFace( Foam::face& face, const std::vector<Foam::label> map ) {
    std::transform( face.begin(), face.end(), face.begin(),
                    [ map ]( const auto& id )
                    { return map[ id ]; } );
}


void Foam::renumberFaces( Foam::faceList& faces, const std::vector<Foam::label> map ) {
    std::for_each( faces.begin(), faces.end(),
                   [ map ]( auto& face )
                   { Foam::renumberFace( face, map ); } );
}
