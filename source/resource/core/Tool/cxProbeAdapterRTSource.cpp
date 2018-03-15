/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/


#include "cxProbeAdapterRTSource.h"
#include <vtkImageChangeInformation.h>
#include <vtkImageData.h>

#include "cxTool.h"
#include "cxTypeConversions.h"
#include "cxLogger.h"
#include "cxData.h"


namespace cx
{

ProbeAdapterRTSource::ProbeAdapterRTSource(QString uid, ProbePtr probe, VideoSourcePtr source) :
	mUid(uid), mBase(source), mProbe(probe)
{
	connect(probe.get(), &Probe::sectorChanged, this, &ProbeAdapterRTSource::probeChangedSlot);

	connect(mBase.get(), &VideoSource::streaming, this, &VideoSource::streaming);
	connect(mBase.get(), &VideoSource::connected, this, &VideoSource::connected);
	connect(mBase.get(), &VideoSource::newFrame, this, &VideoSource::newFrame);
	connect(mBase.get(), &VideoSource::newFrame, this, &ProbeAdapterRTSource::newFrameSlot);

	mRedirecter = vtkImageChangeInformationPtr::New();
	mRedirecter->SetInputData(mBase->getVtkImageData());
	this->probeChangedSlot();
}

ProbeAdapterRTSource::~ProbeAdapterRTSource()
{
	disconnect(mBase.get(), &VideoSource::streaming, this, &VideoSource::streaming);
	disconnect(mBase.get(), &VideoSource::connected, this, &VideoSource::connected);
	disconnect(mBase.get(), &VideoSource::newFrame, this, &VideoSource::newFrame);
	disconnect(mBase.get(), &VideoSource::newFrame, this, &ProbeAdapterRTSource::newFrameSlot);
}

QString ProbeAdapterRTSource::getUid()
{
	return mUid;
}

QString ProbeAdapterRTSource::getName()
{
	return mBase->getName();
}

vtkImageDataPtr ProbeAdapterRTSource::getVtkImageData()
{
	return mRedirecter->GetOutput();
}

double ProbeAdapterRTSource::getTimestamp()
{
	QString uid = mBase->getUid();
	ProbePtr probe = mProbe.lock();
	if (probe)
		return mBase->getTimestamp() - probe->getProbeDefinition(uid).getTemporalCalibration();
	else
		return mBase->getTimestamp();
}

TimeInfo ProbeAdapterRTSource::getAdvancedTimeInfo()
{
	TimeInfo retval;
	retval.mAcquisitionTime.setMSecsSinceEpoch(this->getTimestamp());
	return retval;
}

QString ProbeAdapterRTSource::getInfoString() const
{
	return mBase->getInfoString();
}

QString ProbeAdapterRTSource::getStatusString() const
{
	return mBase->getStatusString();
}

void ProbeAdapterRTSource::start()
{
	mBase->start();
}

void ProbeAdapterRTSource::stop()
{
	mBase->stop();
}

bool ProbeAdapterRTSource::validData() const
{
	return mBase->validData();
}

bool ProbeAdapterRTSource::isConnected() const
{
	return mBase->isConnected();
}

bool ProbeAdapterRTSource::isStreaming() const
{
	return mBase->isStreaming();
}

VideoSourcePtr ProbeAdapterRTSource::getBaseSource()
{
	return mBase;
}

void ProbeAdapterRTSource::newFrameSlot()
{
	ProbePtr probe = mProbe.lock();
	if (!probe)
		return;
	if (!this->validData())
		return;

	mRedirecter->Update();

	QString uid = mBase->getUid();
	ProbeDefinition data = probe->getProbeDefinition(uid);
	QSize dimProbe = data.getSize();
	QSize dimImage(mRedirecter->GetOutput()->GetDimensions()[0], mRedirecter->GetOutput()->GetDimensions()[1]);

	if (dimProbe!=dimImage)
	{
		report(
			QString("Resampling probe calibration. Calibration:[%1,%2], Image:[%3,%4], uid=%5")
			.arg(dimProbe.width())
			.arg(dimProbe.height())
			.arg(dimImage.width())
			.arg(dimImage.height())
		            .arg(uid));

		data.resample(dimImage);
		probe->setProbeDefinition(data);
	}

}

void ProbeAdapterRTSource::probeChangedSlot()
{
	ProbePtr probe = mProbe.lock();
	if (!probe)
		return;

	mRedirecter->Update();

	QString uid = mBase->getUid();
	mRedirecter->SetOutputSpacing(probe->getProbeDefinition(uid).getSpacing().begin());

	mRedirecter->Update();
}

}
