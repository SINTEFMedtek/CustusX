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

#include "cxRegistrationManager.h"

#include <boost/bind.hpp>
#include <ctkPluginContext.h>
#include <QtCore>
#include <QDomElement>
#include "vtkMath.h"
#include "vtkPoints.h"
#include "vtkDoubleArray.h"
#include "vtkLandmarkTransform.h"
#include "vtkMatrix4x4.h"
#include "cxTransform3D.h"
#include "cxRegistrationTransform.h"
#include "cxReporter.h"
#include "cxToolManager.h"
#include "cxDataManager.h"
#include "vesselReg/SeansVesselReg.hxx"
#include "cxPatientService.h"
#include "cxPatientData.h"
#include "cxPluginFramework.h"
#include "cxNullDeleter.h"
#include "cxTypeConversions.h"
#include "cxRegistrationServiceProxy.h"

#include "cxLegacySingletons.h"

namespace cx
{


RegistrationManager::RegistrationManager(RegistrationServicePtr registrationService, AcquisitionDataPtr acquisitionData) :
		mAcquisitionData(acquisitionData),
		mRegistrationService(registrationService)
{
	connect(patientService()->getPatientData().get(), SIGNAL(isSaving()), this, SLOT(duringSavePatientSlot()));
	connect(patientService()->getPatientData().get(), SIGNAL(isLoading()), this, SLOT(duringLoadPatientSlot()));
	connect(patientService()->getPatientData().get(), SIGNAL(cleared()), this, SLOT(clearSlot()));

	connect(mRegistrationService.get(), SIGNAL(fixedDataChanged(QString)), this, SIGNAL(fixedDataChanged(QString)));
	connect(mRegistrationService.get(), SIGNAL(movingDataChanged(QString)), this, SIGNAL(movingDataChanged(QString)));
}

void RegistrationManager::restart()
{
  mRegistrationService->setLastRegistrationTime(QDateTime::currentDateTime());
}

void RegistrationManager::duringSavePatientSlot()
{
	QDomElement managerNode = patientService()->getPatientData()->getCurrentWorkingElement("managers");
  this->addXml(managerNode);
}

void RegistrationManager::duringLoadPatientSlot()
{
	QDomElement registrationManager = patientService()->getPatientData()->getCurrentWorkingElement("managers/registrationManager");
  this->parseXml(registrationManager);
}

DataPtr RegistrationManager::getFixedData()
{
	return mRegistrationService->getFixedData();
}

DataPtr RegistrationManager::getMovingData()
{
	return mRegistrationService->getMovingData();

}

void RegistrationManager::setFixedData(DataPtr fixedData)
{
	return mRegistrationService->setFixedData(fixedData);
}

void RegistrationManager::setMovingData(DataPtr movingData)
{
	return mRegistrationService->setMovingData(movingData);
}

void RegistrationManager::doImageRegistration(bool translationOnly)
{
	mRegistrationService->doImageRegistration(translationOnly);
}

void RegistrationManager::doFastRegistration_Orientation(const Transform3D& tMtm)
{
	Transform3D prMt = toolManager()->getDominantTool()->get_prMt();
	mRegistrationService->doFastRegistration_Orientation(tMtm, prMt);
}


void RegistrationManager::doFastRegistration_Translation()
{
	mRegistrationService->doFastRegistration_Translation();
}

void RegistrationManager::applyPatientOrientation(const Transform3D& tMtm)
{
	Transform3D prMt = toolManager()->getDominantTool()->get_prMt();
	mRegistrationService->applyPatientOrientation(tMtm, prMt);
}

void RegistrationManager::doPatientRegistration()
{
	mRegistrationService->doPatientRegistration();
}

/**\brief apply a new image registration
 *
 * All image registration techniques should use this method
 * to apply the found image registration to the system.
 *
 * The input delta is the
 * \code
 * rM'd = delta * rMd
 * \endcode
 *
 * that updates the existing rMd matrix of the moving image.
 * All related data are also registered.
 */
void RegistrationManager::applyImage2ImageRegistration(Transform3D delta_pre_rMd, QString description)
{
	mRegistrationService->applyImage2ImageRegistration(delta_pre_rMd, description);
}

/**\brief apply a new patient registration
 *
 * All patient registration techniques should use this method
 * to apply the found patient registration to the system.
 *
 */
void RegistrationManager::applyPatientRegistration(Transform3D rMpr_new, QString description)
{
	mRegistrationService->applyPatientRegistration(rMpr_new, description);
}

void RegistrationManager::addXml(QDomNode& parentNode)
{
  QDomDocument doc = parentNode.ownerDocument();
  QDomElement base = doc.createElement("registrationManager");
  parentNode.appendChild(base);

  QDomElement fixedDataNode = doc.createElement("fixedDataUid");
  DataPtr fixedData = this->getFixedData();
  if(fixedData)
  {
	fixedDataNode.appendChild(doc.createTextNode(fixedData->getUid()));
  }
  base.appendChild(fixedDataNode);

  QDomElement movingDataNode = doc.createElement("movingDataUid");
  DataPtr movingData = this->getMovingData();
  if(movingData)
  {
	movingDataNode.appendChild(doc.createTextNode(movingData->getUid()));
  }
  base.appendChild(movingDataNode);
}

void RegistrationManager::parseXml(QDomNode& dataNode)
{
  QString fixedData = dataNode.namedItem("fixedDataUid").toElement().text();
  mRegistrationService->setFixedData(dataManager()->getData(fixedData));

  QString movingData = dataNode.namedItem("movingDataUid").toElement().text();
  mRegistrationService->setMovingData(dataManager()->getData(movingData));
}

void RegistrationManager::clearSlot()
{
  mRegistrationService->setLastRegistrationTime(QDateTime());
  mRegistrationService->setFixedData(DataPtr());
}

}//namespace cx
