#include "cxImageStreamer.h"

namespace cx
{

void ImageStreamer::initialize(StringMap arguments){
	mArguments = arguments;
}

void ImageStreamer::setSendInterval(int milliseconds)
{
	mSendInterval = milliseconds;
}

int ImageStreamer::getSendInterval() const
{
	return mSendInterval;
}
}//namespace cx
