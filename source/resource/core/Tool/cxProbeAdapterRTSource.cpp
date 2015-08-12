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


#include "cxProbeAdapterRTSource.h"
#include <vtkImageChangeInformation.h>
#include <vtkImageData.h>

#include "cxTool.h"
#include "cxTypeConversions.h"
#include "cxLogger.h"


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
		return mBase->getTimestamp() - probe->getProbeDefinition(uid).getTemporalCalibration();
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
	mRedirecter->SetOutputSpacing(probe->getProbeDefinition(uid).getSpacing().begin());

	mRedirecter->Update();
}

}
