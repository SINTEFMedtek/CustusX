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

#include "cxRegistrationImplService.h"

#include "ctkPluginContext.h"
#include "ctkServiceTracker.h"

#include "cxData.h"
#include "cxTypeConversions.h"
#include "cxReporter.h"
#include "cxRegistrationTransform.h"
#include "cxFrameForest.h"
#include "cxPatientModelService.h"

 //TODO: Remove these by moving functionality to PatientModelService
#include "cxLogicManager.h"
#include "cxPatientService.h"
#include "cxDataManager.h"
#include "cxLegacySingletons.h"
#include "cxPatientData.h"
#include "cxRegistrationApplicator.h"

namespace cx
{

RegistrationImplService::RegistrationImplService(ctkPluginContext *context) :
	mContext(context)
{
	mLastRegistrationTime = QDateTime::currentDateTime();
}

RegistrationImplService::~RegistrationImplService()
{
}

void RegistrationImplService::setMovingData(DataPtr movingData)
{
  mMovingData = movingData;
  emit movingDataChanged( (mMovingData) ? qstring_cast(mMovingData->getUid()) : "");
}

void RegistrationImplService::setFixedData(DataPtr fixedData)
{
  if(mFixedData == fixedData)
	return;

  mFixedData = fixedData;
  if (mFixedData)
	report("Registration fixed data set to "+mFixedData->getUid());
  emit fixedDataChanged( (mFixedData) ? qstring_cast(mFixedData->getUid()) : "");
}

DataPtr RegistrationImplService::getMovingData()
{
  return mMovingData;
}

DataPtr RegistrationImplService::getFixedData()
{
  return mFixedData;
}

QDateTime RegistrationImplService::getLastRegistrationTime()
{
	return mLastRegistrationTime;
}


void RegistrationImplService::setLastRegistrationTime(QDateTime time)
{
	mLastRegistrationTime = time;
}

void RegistrationImplService::applyImage2ImageRegistration(Transform3D delta_pre_rMd, QString description)
{
	RegistrationTransform regTrans(delta_pre_rMd, QDateTime::currentDateTime(), description);
	regTrans.mFixed = mFixedData ? mFixedData->getUid() : "";
	regTrans.mMoving = mMovingData ? mMovingData->getUid() : "";
	this->updateRegistration(mLastRegistrationTime, regTrans, mMovingData, regTrans.mFixed);
	mLastRegistrationTime = regTrans.mTimestamp;
	reportSuccess(QString("Image registration [%1] has been performed on %2").arg(description).arg(regTrans.mMoving) );
//	patientService()->getPatientData()->autoSave();

	cx::logicManager()->getPatientService()->getPatientData()->autoSave();//TODO
}

PatientModelService* RegistrationImplService::getPatientModelService()
{
	ctkServiceTracker<PatientModelService*> tracker(mContext);
	tracker.open();
	PatientModelService* service = tracker.getService(); // get arbitrary instance of this type
	if(!service)
		reportError("RegistrationImplService can't access PatientModelService");
	return service;
}

void RegistrationImplService::applyPatientRegistration(Transform3D rMpr_new, QString description)
{
	RegistrationTransform regTrans(rMpr_new, QDateTime::currentDateTime(), description);
	regTrans.mFixed = mFixedData ? mFixedData->getUid() : "";
	this->getPatientModelService()->updateRegistration_rMpr(mLastRegistrationTime, regTrans);
	mLastRegistrationTime = regTrans.mTimestamp;
	reportSuccess(QString("Patient registration [%1] has been performed.").arg(description));
}

/** Update the registration for data and all data connected to its space.
 *
 * Registration is done relative to masterFrame, i.e. data is moved relative to the masterFrame.
 *
 */
void RegistrationImplService::updateRegistration(QDateTime oldTime, RegistrationTransform deltaTransform, DataPtr data, QString masterFrameUid)
{
	RegistrationApplicator applicator(dataService()->getData());
	applicator.updateRegistration(oldTime, deltaTransform, data, masterFrameUid);
}

bool RegistrationImplService::isNull()
{
	return false;
}

} /* namespace cx */
