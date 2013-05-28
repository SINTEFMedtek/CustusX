#include "cxDirectlyLinkedSender.h"

#include "cxIGTLinkConversion.h"

namespace cx
{

bool DirectlyLinkedSender::isReady() const
{
	return true;
}

void DirectlyLinkedSender::send(IGTLinkImageMessage::Pointer msg)
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

void DirectlyLinkedSender::send(IGTLinkUSStatusMessage::Pointer msg)
{
	if (!msg || !this->isReady())
		return;
	mUnsentUSStatusMessage = msg;
}

void DirectlyLinkedSender::send(ssc::ImagePtr msg)
{
	if (!this->isReady())
		return;

	mImage = msg;
	// decode color format:
	IGTLinkConversion converter;
	mImage = converter.decode(msg);

	emit newImage();
}

void DirectlyLinkedSender::send(ssc::ProbeDataPtr msg)
{
	if (!this->isReady())
		return;
	// decode color format:
	IGTLinkConversion converter;
	mUSStatus = converter.decode(msg);
	emit newUSStatus();
}

ssc::ImagePtr DirectlyLinkedSender::popImage()
{
	return mImage;
	mImage.reset();
}
ssc::ProbeDataPtr DirectlyLinkedSender::popUSStatus()
{
	return mUSStatus;
}

}
