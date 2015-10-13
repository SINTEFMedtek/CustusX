#include "cxEncodedPackage.h"
#include "cxLogger.h"

namespace cx {

EncodedPackage::EncodedPackage(char *pointer, int size) :
    mData(new ByteArray())
{
//    CX_LOG_DEBUG() << "Preparing pack, pointer: " << pointer << " size: " << size;
    mData->pointer = pointer;
    mData->size = size;
}

} //namespace cx

