#include "cxGrabberSenderDirectLink.h"

#include "cxIGTLinkConversion.h"

namespace cx
{

bool GrabberSenderDirectLink::isReady() const
{
	return true;
}

void GrabberSenderDirectLink::send(IGTLinkImageMessage::Pointer msg)
{
	if (!msg || !this->isReady())
		return;
	IGTLinkConversion converter;
	this->send(converter.decode(msg));
	if (mUnsentUSStatusMessage)
	{
		this->send(converter.decode(mUnsentUSStatusMessage, msg, mUSStatus));
		mUnsentUSStatusMessage = IGTLinkUSStatusMessage::Pointer();
	}
}

void GrabberSenderDirectLink::send(IGTLinkUSStatusMessage::Pointer msg)
{
	if (!msg || !this->isReady())
		return;
	mUnsentUSStatusMessage = msg;
}

void GrabberSenderDirectLink::send(ssc::ImagePtr msg)
{
	if (!this->isReady())
		return;

	mImage = msg;
	// decode color format:
	IGTLinkConversion converter;
	mImage = converter.decode(msg);

	emit newImage();
}

void GrabberSenderDirectLink::send(ssc::ProbeData msg)
{
	if (!this->isReady())
		return;
	// decode color format:
	IGTLinkConversion converter;
	mUSStatus = converter.decode(msg);
	emit newUSStatus();
}

ssc::ImagePtr GrabberSenderDirectLink::popImage()
{
	return mImage;
	mImage.reset();
}
ssc::ProbeData GrabberSenderDirectLink::popUSStatus()
{
	return mUSStatus;
//	mUSStatus = IGTLinkUSStatusMessage::Pointer();
}

}
