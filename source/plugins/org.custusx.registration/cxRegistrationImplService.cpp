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
#include "cxData.h"
#include "cxTypeConversions.h"
#include "cxReporter.h"

#include "cxRegistrationTransform.h"
#include "cxFrameForest.h"

 //TODO: Remove these by moving functionality to PatientModelService
#include "cxLogicManager.h"
#include "cxPatientService.h"
#include "cxDataManager.h"
#include "cxLegacySingletons.h"
#include "cxPatientData.h"

namespace cx
{

RegistrationImplService::RegistrationImplService()
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

void RegistrationImplService::applyPatientRegistration(Transform3D rMpr_new, QString description)
{
	RegistrationTransform regTrans(rMpr_new, QDateTime::currentDateTime(), description);
	regTrans.mFixed = mFixedData ? mFixedData->getUid() : "";
	dataManager()->get_rMpr_History()->updateRegistration(mLastRegistrationTime, regTrans);
	mLastRegistrationTime = regTrans.mTimestamp;
	reportSuccess(QString("Patient registration [%1] has been performed.").arg(description));
	patientService()->getPatientData()->autoSave();
}

/** Update the registration for data and all data connected to its space.
 *
 * Registration is done relative to masterFrame, i.e. data is moved relative to the masterFrame.
 *
 */
void RegistrationImplService::updateRegistration(QDateTime oldTime, RegistrationTransform deltaTransform, DataPtr data, QString masterFrameUid)
{
//	std::cout << "==== RegistrationManager::updateRegistration" << std::endl;
	FrameForest forest(dataService());
  QDomNode target = forest.getNode(qstring_cast(data->getUid()));
  QDomNode masterFrame = target;
  QDomNode targetBase = target;
  if (masterFrameUid!="")
  {
	  masterFrame = forest.getNode(masterFrameUid);
	  targetBase = forest.getOldestAncestorNotCommonToRef(target, masterFrame);
  }
  std::vector<DataPtr> targetData = forest.getDataFromDescendantsAndSelf(targetBase);

  std::stringstream ss;
  ss << "Update Registration using " << std::endl;
  ss << "\tFixed:\t" << masterFrameUid << std::endl;
  ss << "\tMoving:\t" << data->getUid() << std::endl;
  ss << "\tDelta matrix (rMd'=Delta*rMd)\n"+qstring_cast(deltaTransform.mValue) << std::endl;
  report(qstring_cast(ss.str()));

  // update the transform on all target data:
  for (unsigned i=0; i<targetData.size(); ++i)
  {
	RegistrationTransform newTransform = deltaTransform;
	newTransform.mValue = deltaTransform.mValue * targetData[i]->get_rMd();
	targetData[i]->get_rMd_History()->updateRegistration(oldTime, newTransform);

	report("Updated registration of data " + targetData[i]->getName());
	//std::cout << "rMd_new\n" << newTransform.mValue << std::endl; // too much noise for large patients
  }

  // reconnect only if master and target are unconnected, i.e. share a common ancestor.
  // If we are registrating inside an already connected tree we only want to change transforms,
  // not change the topology of the tree.
  if (forest.getOldestAncestor(target) != forest.getOldestAncestor(masterFrame))
  {
	// connect the target to the master's ancestor, i.e. replace targetBase with masterAncestor:
	QDomNode masterAncestor = forest.getOldestAncestor(masterFrame);
	// iterate over all target data,
	for (unsigned i=0; i<targetData.size(); ++i)
	{
	  QString masterAncestorUid = masterAncestor.toElement().tagName();
	  QString targetBaseUid = targetBase.toElement().tagName();

	  if (targetData[i]->getParentSpace() == targetBaseUid)
	  {
		report("Reset parent frame of " + targetData[i]->getName() + " to " + masterAncestorUid + ". targetbase=" + targetBaseUid);
		targetData[i]->get_rMd_History()->updateParentSpace(oldTime, ParentSpace(masterAncestorUid, deltaTransform.mTimestamp, deltaTransform.mType));
	  }
	}
  }
}

bool RegistrationImplService::isNull()
{
	return false;
}

} /* namespace cx */
