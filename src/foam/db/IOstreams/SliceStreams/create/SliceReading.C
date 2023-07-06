
#include "SliceReading.H"

#include "InputFeatures.H"
#include "FileSliceStream.H"


std::unique_ptr<Foam::SliceStream>
Foam::SliceReading::createStream()
{
    std::unique_ptr<Foam::StreamFeatures>
    file = std::make_unique<Foam::InputFeatures>();
    return std::make_unique<Foam::FileSliceStream>(file);
}


