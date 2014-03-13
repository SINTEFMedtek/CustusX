// This file is part of SSC,
// a C++ Library supporting Image Guided Therapy Applications.
//
// Copyright (C) 2008- SINTEF Medical Technology
// Copyright (C) 2008- Sonowand AS
//
// SSC is owned by SINTEF Medical Technology and Sonowand AS,
// hereafter named the owners. Each particular piece of code
// is owned by the part that added it to the library.
// SSC source code and binaries can only be used by the owners
// and those with explicit permission from the owners.
// SSC shall not be distributed to anyone else.
//
// SSC is distributed WITHOUT ANY WARRANTY; without even
// the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE.
//
// See sscLicense.txt for more information.

#include "cxProbeAdapterRTSource.h"
#include <vtkImageChangeInformation.h>
#include <vtkImageData.h>
#include "cxReporter.h"
#include "cxTool.h"
#include "cxTypeConversions.h"

namespace cx
{

ProbeAdapterRTSource::ProbeAdapterRTSource(QString uid, ProbePtr probe, VideoSourcePtr source) :
	mUid(uid), mBase(source), mProbe(probe)
{
	connect(probe.get(), SIGNAL(sectorChanged()), this, SLOT(probeChangedSlot()));

	connect(mBase.get(), SIGNAL(streaming(bool)), this, SIGNAL(streaming(bool)));
	connect(mBase.get(), SIGNAL(connected(bool)), this, SIGNAL(connected(bool)));
	connect(mBase.get(), SIGNAL(newFrame()), this, SIGNAL(newFrame()));
	connect(mBase.get(), SIGNAL(newFrame()), this, SLOT(newFrameSlot()));

	mRedirecter = vtkImageChangeInformationPtr::New();
	mRedirecter->SetInput(mBase->getVtkImageData());
	this->probeChangedSlot();
}

ProbeAdapterRTSource::~ProbeAdapterRTSource()
{}

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
		return mBase->getTimestamp() - probe->getProbeData(uid).getTemporalCalibration();
	else
		return mBase->getTimestamp();
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
	ProbeDefinition data = probe->getProbeData(uid);
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
		probe->setProbeSector(data);
	}

}

void ProbeAdapterRTSource::probeChangedSlot()
{
	ProbePtr probe = mProbe.lock();
	if (!probe)
		return;

	mRedirecter->Update();

	QString uid = mBase->getUid();
	mRedirecter->SetOutputSpacing(probe->getProbeData(uid).getSpacing().begin());

	mRedirecter->Update();
}

}
