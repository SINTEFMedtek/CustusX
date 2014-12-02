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

#include "cxAcquisitionImplService.h"
#include "cxAcquisitionData.h"
#include "cxUSAcquisition.h"
#include "cxUsReconstructionServiceProxy.h"

namespace cx
{

AcquisitionImplService::AcquisitionImplService(ctkPluginContext *context) :
	mContext(context),
	mAcquisitionData(new AcquisitionData()),
	mAcquisition(new Acquisition(mAcquisitionData)),
	mUsReconstructService(new UsReconstructionServiceProxy(context)),
	mUsAcquisition(new USAcquisition(mAcquisition, mUsReconstructService))
{
	connect(mAcquisition.get(), &Acquisition::started, this, &AcquisitionService::started);
	connect(mAcquisition.get(), &Acquisition::cancelled, this, &AcquisitionService::cancelled);
	connect(mAcquisition.get(), &Acquisition::stateChanged, this, &AcquisitionService::stateChanged);
	connect(mAcquisition.get(), &Acquisition::readinessChanged, this, &AcquisitionService::readinessChanged);
	connect(mAcquisition.get(), &Acquisition::acquisitionStopped, this, &AcquisitionService::acquisitionStopped);

	connect(mAcquisitionData.get(), &AcquisitionData::recordedSessionsChanged, this, &AcquisitionService::recordedSessionsChanged);

	connect(mUsAcquisition.get(), &USAcquisition::acquisitionDataReady, this, &AcquisitionService::acquisitionDataReady);
	connect(mUsAcquisition.get(), &USAcquisition::saveDataCompleted, this, &AcquisitionService::saveDataCompleted);
}

AcquisitionImplService::~AcquisitionImplService()
{
	disconnect(mAcquisition.get(), &Acquisition::started, this, &AcquisitionService::started);
	disconnect(mAcquisition.get(), &Acquisition::cancelled, this, &AcquisitionService::cancelled);
	disconnect(mAcquisition.get(), &Acquisition::stateChanged, this, &AcquisitionService::stateChanged);
	disconnect(mAcquisition.get(), &Acquisition::readinessChanged, this, &AcquisitionService::readinessChanged);
	disconnect(mAcquisition.get(), &Acquisition::acquisitionStopped, this, &AcquisitionService::acquisitionStopped);

	disconnect(mAcquisitionData.get(), &AcquisitionData::recordedSessionsChanged, this, &AcquisitionService::recordedSessionsChanged);

	disconnect(mUsAcquisition.get(), &USAcquisition::acquisitionDataReady, this, &AcquisitionService::acquisitionDataReady);
	disconnect(mUsAcquisition.get(), &USAcquisition::saveDataCompleted, this, &AcquisitionService::saveDataCompleted);
}

bool AcquisitionImplService::isNull()
{
	return false;
}

RecordSessionPtr AcquisitionImplService::getLatestSession()
{
	return mAcquisition->getLatestSession();
}

std::vector<RecordSessionPtr> AcquisitionImplService::getSessions()
{
	return mAcquisition->getPluginData()->getRecordSessions();
}

bool AcquisitionImplService::isReady() const
{
	return mAcquisition->isReady();
}

QString AcquisitionImplService::getInfoText() const
{
	return mAcquisition->getInfoText();
}

AcquisitionService::STATE AcquisitionImplService::getState() const
{
	return mAcquisition->getState();
}

void AcquisitionImplService::toggleRecord()
{
	mAcquisition->toggleRecord();
}

void AcquisitionImplService::startRecord()
{
	mAcquisition->startRecord();
}

void AcquisitionImplService::stopRecord()
{
	mAcquisition->stopRecord();
}

void AcquisitionImplService::cancelRecord()
{
	mAcquisition->cancelRecord();
}

void AcquisitionImplService::startPostProcessing()
{
	mAcquisition->startPostProcessing();
}

void AcquisitionImplService::stopPostProcessing()
{
	mAcquisition->stopPostProcessing();
}

int AcquisitionImplService::getNumberOfSavingThreads() const
{
	return mUsAcquisition->getNumberOfSavingThreads();
}

void AcquisitionImplService::addXml(QDomNode &dataNode)
{
	mAcquisitionData->addXml(dataNode);
}

void AcquisitionImplService::parseXml(QDomNode &dataNode)
{
	mAcquisitionData->parseXml(dataNode);
}

} // cx
