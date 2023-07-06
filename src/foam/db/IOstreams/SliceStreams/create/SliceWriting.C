
#include "SliceWriting.H"

#include "OutputFeatures.H"
#include "FileSliceStream.H"


std::unique_ptr<Foam::SliceStream>
Foam::SliceWriting::createStream()
{
    std::unique_ptr<Foam::StreamFeatures>
    file = std::make_unique<Foam::OutputFeatures>();
    return std::make_unique<Foam::FileSliceStream>(file);
}


