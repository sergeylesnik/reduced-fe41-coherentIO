
#include "sliceMeshHelper.H"

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
