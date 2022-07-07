
#include "fileName.H"

namespace Foam {

class adiosPaths {

    const Foam::fileName meshPathname_{ "constant/polyMesh.bp" };
    const Foam::fileName dataPathname_{ "data.bp" };
    Foam::fileName pathname_{ "data.bp" };
    bool filesChecked_{ false };
    bool dataPresent_{ false };
    bool meshPresent_{ false };

    void checkFiles();

public:

    void setPathName( const fileName& );

    const fileName& getPathName() const;

    const fileName& meshPathname();

    const fileName& dataPathname();

    bool dataPresent();

    bool meshPresent();

};

}
