/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxStreamer.h"
#include "cxSender.h"

namespace cx
{

Streamer::Streamer() :
		QObject(NULL), mSendInterval(0), mSendTimer(0), mInitialized(false)
{}

void Streamer::setSendInterval(int milliseconds)
{
	mSendInterval = milliseconds;
}

int Streamer::getSendInterval() const
{
	return mSendInterval;
}

void Streamer::setInitialized(bool initialized)
{
	mInitialized = initialized;
}

bool Streamer::isInitialized()
{
	return mInitialized;
}

void Streamer::createSendTimer(bool singleshot)
{
	mSendTimer = new QTimer(this);
	mSendTimer->setSingleShot(singleshot);
	connect(mSendTimer, SIGNAL(timeout()), this, SLOT(streamSlot()));
}

bool Streamer::isReadyToSend()
{
	return mSender && mSender->isReady();
}

//--------------------------------------------------------


//--------------------------------------------------------

void CommandLineStreamer::initialize(StringMap arguments)
{
	mArguments = arguments;
}

}//namespace cx
