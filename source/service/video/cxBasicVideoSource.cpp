// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.


#include "cxBasicVideoSource.h"

#include <QTimer>
#include <vtkImageChangeInformation.h>
#include <vtkImageData.h>
#include "sscImage.h"
#include "sscBoundingBox3D.h"
#include "sscMessageManager.h"
#include "sscVolumeHelpers.h"

namespace cx
{

BasicVideoSource::BasicVideoSource()
{
	mRedirecter = vtkSmartPointer<vtkImageChangeInformation>::New(); // used for forwarding only.


	vtkImageDataPtr emptyImage = ssc::generateVtkImageData(Eigen::Array3i(1,1,1),
														   ssc::Vector3D(1,1,1),
														   0);
	mEmptyImage.reset(new ssc::Image("<none>", emptyImage));
	mReceivedImage = mEmptyImage;
	mRedirecter->SetInput(mEmptyImage->getBaseVtkImageData());

	mTimeout = true; // must start invalid
	mTimeoutTimer = new QTimer(this);
	mTimeoutTimer->setInterval(1000);
	connect(mTimeoutTimer, SIGNAL(timeout()), this, SLOT(timeout()));
//	connect(this, SIGNAL(connected(bool)), this, SIGNAL(streaming(bool))); // define connected as streaming.
}

BasicVideoSource::~BasicVideoSource()
{
	stop();
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

	ssc::messageManager()->sendWarning("Timeout!");
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
	mTimeoutTimer->start();

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
	mTimeoutTimer->stop();

	emit streaming(false);
	emit newFrame();
}

void BasicVideoSource::setInput(ssc::ImagePtr input)
{
	bool wasConnected = this->isConnected();
	mReceivedImage = input;
	if (!mReceivedImage)
		mReceivedImage = mEmptyImage;

	mTimeout = false;
	mTimeoutTimer->start();

	if (this->isConnected() != wasConnected)
		emit connected(this->isConnected());

	emit newFrame();
}

} // namespace cx
