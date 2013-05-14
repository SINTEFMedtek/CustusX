#include "cxImageStreamer.h"

namespace cx
{

void ImageStreamer::initialize(StringMap arguments){
	mArguments = arguments;
}

void ImageStreamer::setSendInterval(int interval)
{
	mSendInterval = interval;
}

int ImageStreamer::getSendInterval() const
{
	return mSendInterval;
}
}//namespace cx
