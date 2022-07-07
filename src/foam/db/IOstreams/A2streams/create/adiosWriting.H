
#ifndef adiosWriting_H
#define adiosWriting_H

#include "adiosStreamCreator.H"

namespace Foam 
{

class adiosStream;

struct adiosWriting : public adiosStreamCreator {

    virtual std::unique_ptr<adiosStream> createStream() override;

};

}

#endif

