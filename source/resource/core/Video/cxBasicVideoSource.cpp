/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
=========================================================================*/


#include "cxBasicVideoSource.h"

#include <QTimer>
#include <vtkImageChangeInformation.h>
#include <vtkImageData.h>
#include "cxImage.h"
#include "cxBoundingBox3D.h"
#include "cxLogger.h"
#include "cxVolumeHelpers.h"
#include "cxTypeConversions.h"


namespace cx
{

BasicVideoSource::BasicVideoSource(QString uid) :
	mStreaming(false)
{
	mStatus = "USE_DEFAULT";
	mRedirecter = vtkSmartPointer<vtkImageChangeInformation>::New(); // used for forwarding only.


	vtkImageDataPtr emptyImage = generateVtkImageData(Eigen::Array3i(3,3,1),
														   Vector3D(1,1,1),
														   0);
	mEmptyImage.reset(new Image(uid, emptyImage));
	mReceivedImage = mEmptyImage;
	mRedirecter->SetInputData(mEmptyImage->getBaseVtkImageData());

	mTimeout = true; // must start invalid
	mTimeoutTimer = new QTimer(this);
	mTimeoutTimer->setInterval(1000);
	connect(mTimeoutTimer, SIGNAL(timeout()), this, SLOT(timeout()));
}

BasicVideoSource::~BasicVideoSource()
{
	stop();
}

void BasicVideoSource::overrideTimeout(bool timeout)
{
	if (mTimeoutTimer)
	{
		mTimeoutTimer->setParent(NULL);
		delete mTimeoutTimer;
		mTimeoutTimer = NULL;
	}

	mTimeout = timeout;
}

QString BasicVideoSource::getUid()
{
	return mReceivedImage->getUid();
}
QString BasicVideoSource::getName()
{
	return mReceivedImage->getName();
}

void BasicVideoSource::timeout()
{
	if (mTimeout)
		return;

	reportWarning("Timeout!");
	mTimeout = true;
	emit newFrame();
}

bool BasicVideoSource::validData() const
{
	return this->isConnected() && !mTimeout;
}

double BasicVideoSource::getTimestamp()
{
	return mReceivedImage->getAcquisitionTime().toMSecsSinceEpoch();
}

TimeInfo BasicVideoSource::getAdvancedTimeInfo()
{
	return mReceivedImage->getAdvancedTimeInfo();
}

bool BasicVideoSource::isConnected() const
{
	return (mReceivedImage!=mEmptyImage);
}

bool BasicVideoSource::isStreaming() const
{
	return this->isConnected() && mStreaming;
}

void BasicVideoSource::setResolution(double resolution)
{
	mRedirecter->SetOutputSpacing(resolution, resolution, resolution);
}

vtkImageDataPtr BasicVideoSource::getVtkImageData()
{
	mRedirecter->Update();
	return mRedirecter->GetOutput();
}

void BasicVideoSource::start()
{
	if (mStreaming)
		return;

	mStreaming = true;

	if (mTimeoutTimer)
	{
		mTimeoutTimer->start();
	}

	if (!this->isConnected())
		return;

	emit streaming(true);
	emit newFrame();
}

void BasicVideoSource::stop()
{
	if (!mStreaming)
		return;

	mStreaming = false;
	if (mTimeoutTimer)
	{
		mTimeoutTimer->stop();
	}

	emit streaming(false);
	emit newFrame();
}

QString BasicVideoSource::getStatusString() const
{
	if (mStatus!="USE_DEFAULT")
		return mStatus;

//	 { return mStatus; }
	if (!this->isConnected())
		return "Not connected";
	if (!this->isStreaming())
		return "Not streaming";
	if (!this->validData())
		return "Timeout";
//	return "Running";
	return "";
}


void BasicVideoSource::setInput(ImagePtr input)
{
	bool wasConnected = this->isConnected();

	if (input)
	{
		mReceivedImage = input;
	}
	else
	{
		if (mReceivedImage)
		{
			// create an empty image with the same uid as the stream.
			mEmptyImage.reset(new Image(mReceivedImage->getUid(), mEmptyImage->getBaseVtkImageData()));
		}
		mReceivedImage = mEmptyImage;
	}
	mRedirecter->SetInputData(mReceivedImage->getBaseVtkImageData());
	mRedirecter->Update();

	if (mTimeoutTimer)
	{
		mTimeout = false;
		mTimeoutTimer->start();
	}

	if (this->isConnected() != wasConnected)
		emit connected(this->isConnected());

	emit newFrame();
}

} // namespace cx
