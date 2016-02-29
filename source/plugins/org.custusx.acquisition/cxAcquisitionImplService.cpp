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
#include "cxPatientModelServiceProxy.h"
#include "cxSessionStorageServiceProxy.h"
#include "cxXMLNodeWrapper.h"
#include "cxVisServices.h"

namespace cx
{

//TESTING :)

AcquisitionImplService::AcquisitionImplService(ctkPluginContext *context) :
	mContext(context),
	mUsReconstructService(new UsReconstructionServiceProxy(context))
{
	mServices = VisServices::create(context);
	mAcquisitionData.reset(new AcquisitionData(mServices, mUsReconstructService));
	mAcquisition.reset(new Acquisition(mAcquisitionData));
	mUsAcquisition.reset(new USAcquisition(mAcquisition));

	connect(mAcquisition.get(), &Acquisition::started, this, &AcquisitionService::started);
	connect(mAcquisition.get(), &Acquisition::cancelled, this, &AcquisitionService::cancelled);
	connect(mAcquisition.get(), &Acquisition::stateChanged, this, &AcquisitionService::stateChanged);
	connect(mAcquisition.get(), &Acquisition::acquisitionStopped, this, &AcquisitionService::acquisitionStopped);
	connect(mAcquisition.get(), &Acquisition::readinessChanged, this, &AcquisitionService::usReadinessChanged);

	connect(mAcquisitionData.get(), &AcquisitionData::recordedSessionsChanged, this, &AcquisitionService::recordedSessionsChanged);

	connect(mUsAcquisition.get(), &USAcquisition::acquisitionDataReady, this, &AcquisitionService::acquisitionDataReady);
	connect(mUsAcquisition.get(), &USAcquisition::saveDataCompleted, this, &AcquisitionService::saveDataCompleted);
	connect(mUsAcquisition.get(), &USAcquisition::readinessChanged, this, &AcquisitionService::usReadinessChanged);

	connect(mServices->session().get(), &SessionStorageService::cleared, this, &AcquisitionImplService::duringClearPatientSlot);
	connect(mServices->session().get(), &SessionStorageService::isLoading, this, &AcquisitionImplService::duringLoadPatientSlot);
	connect(mServices->session().get(), &SessionStorageService::isSaving, this, &AcquisitionImplService::duringSavePatientSlot);
}

AcquisitionImplService::~AcquisitionImplService()
{
}

bool AcquisitionImplService::isNull()
{
	return false;
}

void AcquisitionImplService::duringClearPatientSlot()
{
	// clear data?
	mAcquisitionData->clear();//TODO: Check if wee need this
}

void AcquisitionImplService::duringSavePatientSlot(QDomElement& node)
{
	XMLNodeAdder root(node);
	QDomElement managerNode = root.descend("managers").node().toElement();
	this->addXml(managerNode);
}

void AcquisitionImplService::duringLoadPatientSlot(QDomElement& node)
{
	XMLNodeParser root(node);
	QDomElement stateManagerNode = root.descend("managers/stateManager").node().toElement();
	if (!stateManagerNode.isNull())
		this->parseXml(stateManagerNode);
}

RecordSessionPtr AcquisitionImplService::getLatestSession()
{
	return mAcquisition->getLatestSession();
}

std::vector<RecordSessionPtr> AcquisitionImplService::getSessions()
{
	return mAcquisition->getPluginData()->getRecordSessions();
}

bool AcquisitionImplService::isReady(TYPES context) const
{
	return mUsAcquisition->isReady(context)
			&& mAcquisition->isReady(context);
}

QString AcquisitionImplService::getInfoText(TYPES context) const
{
	QString text;
	text = mAcquisition->getInfoText(context) + "<br>" + mUsAcquisition->getInfoText(context);

	// remove redundant line breaks
	QStringList list = text.split("<br>", QString::SkipEmptyParts);
	text = list.join("<br>");

	//Make sure we have at least 2 lines to avoid "bouncing buttons"
	if (list.size() < 2)
		text.append("<br>");

	return text;
}

AcquisitionService::STATE AcquisitionImplService::getState() const
{
	return mAcquisition->getState();
}

void AcquisitionImplService::startRecord(TYPES context, QString category, RecordSessionPtr session)
{
	mAcquisition->startRecord(context, category, session);
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
