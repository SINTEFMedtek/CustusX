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

/*
 * cxProbeAdapterRTSource.cpp
 *
 *  Created on: Feb 11, 2011
 *      Author: christiana
 */

#include "sscProbeAdapterRTSource.h"
#include <vtkImageChangeInformation.h>
#include <vtkImageData.h>
#include "sscMessageManager.h"
#include "sscTool.h"
#include "sscTypeConversions.h"

namespace ssc
{

ProbeAdapterRTSource::ProbeAdapterRTSource(QString uid, ssc::ProbePtr probe, ssc::VideoSourcePtr source) :
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

QString ProbeAdapterRTSource::getUid()
{
	return mUid;
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

/**called when a new frame arrives.
 *
 * Check the image sizes of the probe definition and the
 * image. If different: resample the probe def to match
 * the image.
 *
 */
void ProbeAdapterRTSource::newFrameSlot()
{
	ProbePtr probe = mProbe.lock();
	if (!probe)
		return;
	if (!this->validData())
		return;

	mRedirecter->Update();

	QString uid = mBase->getUid();
	ssc::ProbeData data = probe->getProbeData(uid);
	QSize dimProbe = data.getImage().mSize;
	QSize dimImage(mRedirecter->GetOutput()->GetDimensions()[0], mRedirecter->GetOutput()->GetDimensions()[1]);

	if (dimProbe!=dimImage)
	{
		ssc::messageManager()->sendInfo(
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
	mRedirecter->SetOutputSpacing(probe->getProbeData(uid).getImage().mSpacing.begin());

	mRedirecter->Update();
}

}
