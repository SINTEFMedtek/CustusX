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
#include "cxLandmarkTranslationRegistration.h"
#include "vesselReg/SeansVesselReg.hxx"
#include "cxPatientService.h"
#include "cxPatientData.h"
#include "cxPluginFramework.h"
#include "cxNullDeleter.h"
#include "cxTypeConversions.h"
#include "cxRegistrationServiceProxy.h"

namespace cx
{


RegistrationManager::RegistrationManager(AcquisitionDataPtr acquisitionData, ctkPluginContext* pluginContext) :
		mAcquisitionData(acquisitionData),
		mRegistrationService(new cx::RegistrationServiceProxy(pluginContext))
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


/**Inspect the landmarks in data a and b, find landmarks defined in both of them and
 * that also is active.
 * Return the uids of these landmarks.
 */
std::vector<QString> RegistrationManager::getUsableLandmarks(const LandmarkMap& data_a, const LandmarkMap& data_b)
{
  std::vector<QString> retval;
  std::map<QString, LandmarkProperty> props = dataManager()->getLandmarkProperties();
  std::map<QString, LandmarkProperty>::iterator iter;

  for (iter=props.begin(); iter!=props.end(); ++iter)
  {
    QString uid = iter->first;
    if (data_a.count(uid) && data_b.count(uid) && iter->second.getActive())
      retval.push_back(uid);
  }
  return retval;
}

/**Convert the landmarks given by uids to vtk points.
 * The coordinates are given by the input data,
 * and should be transformed by M.
 *
 * Prerequisite: all uids exist in data.
 */
vtkPointsPtr RegistrationManager::convertTovtkPoints(const std::vector<QString>& uids, const LandmarkMap& data, Transform3D M)
{
  vtkPointsPtr retval = vtkPointsPtr::New();

  for (unsigned i=0; i<uids.size(); ++i)
  {
    QString uid = uids[i];
    Vector3D p = M.coord(data.find(uid)->second.getCoord());
    retval->InsertNextPoint(p.begin());
  }
  return retval;
}

std::vector<Vector3D> RegistrationManager::convertAndTransformToPoints(const std::vector<QString>& uids, const LandmarkMap& data, Transform3D M)
{
  std::vector<Vector3D> retval;

  for (unsigned i=0; i<uids.size(); ++i)
  {
    QString uid = uids[i];
    Vector3D p = M.coord(data.find(uid)->second.getCoord());
    retval.push_back(p);
  }
  return retval;
}

std::vector<Vector3D> RegistrationManager::convertVtkPointsToPoints(vtkPointsPtr base)
{
  std::vector<Vector3D> retval;

  for (int i=0; i<base->GetNumberOfPoints(); ++i)
  {
    Vector3D p(base->GetPoint(i));
    retval.push_back(p);
  }
  return retval;
}

/** Perform a landmark registration between the data sets source and target.
 *  Return transform from source to target.
 */
Transform3D RegistrationManager::performLandmarkRegistration(vtkPointsPtr source, vtkPointsPtr target, bool* ok) const
{
  *ok = false;

  // too few data samples: ignore
  if (source->GetNumberOfPoints() < 3)
  {
    return Transform3D::Identity();
  }

  vtkLandmarkTransformPtr landmarktransform = vtkLandmarkTransformPtr::New();
  landmarktransform->SetSourceLandmarks(source);
  landmarktransform->SetTargetLandmarks(target);
  landmarktransform->SetModeToRigidBody();
  source->Modified();
  target->Modified();
  landmarktransform->Update();

  Transform3D tar_M_src(landmarktransform->GetMatrix());

  if (QString::number(tar_M_src(0,0))=="nan") // harry but quick way to check badness of transform...
  {
    return Transform3D::Identity();
  }

  *ok = true;
  return tar_M_src;
}

void RegistrationManager::doPatientRegistration()
{
  DataPtr fixedImage = this->getFixedData();

  if(!fixedImage)
  {
	reportError("The fixed data is not set, cannot do patient registration!");
    return;
  }
  LandmarkMap fixedLandmarks = fixedImage->getLandmarks()->getLandmarks();
  LandmarkMap toolLandmarks = dataManager()->getPatientLandmarks()->getLandmarks();

  this->writePreLandmarkRegistration(fixedImage->getName(), fixedImage->getLandmarks()->getLandmarks());
  this->writePreLandmarkRegistration("physical", toolLandmarks);

  std::vector<QString> landmarks = this->getUsableLandmarks(fixedLandmarks, toolLandmarks);

  vtkPointsPtr p_ref = this->convertTovtkPoints(landmarks, fixedLandmarks, fixedImage->get_rMd());
  vtkPointsPtr p_pr = this->convertTovtkPoints(landmarks, toolLandmarks, Transform3D::Identity());

  // ignore if too few data.
  if (p_ref->GetNumberOfPoints() < 3)
    return;

  bool ok = false;
  Transform3D rMpr = this->performLandmarkRegistration(p_pr, p_ref, &ok);
  if (!ok)
  {
    reportError("P-I Landmark registration: Failed to register: [" + qstring_cast(p_pr->GetNumberOfPoints()) + "p]");
    return;
  }

  this->applyPatientRegistration(rMpr, "Patient Landmark");
}

void RegistrationManager::writePreLandmarkRegistration(QString name, LandmarkMap landmarks)
{
	QStringList lm;
	for (LandmarkMap::iterator iter=landmarks.begin(); iter!=landmarks.end(); ++iter)
	{
		lm << dataManager()->getLandmarkProperties()[iter->second.getUid()].getName();
	}

	QString msg = QString("Preparing to register [%1] containing the landmarks: [%2]").arg(name).arg(lm.join(","));
	report(msg);
}

void RegistrationManager::doImageRegistration(bool translationOnly)
{
  //check that the fixed data is set
  DataPtr fixedImage = this->getFixedData();
  if(!fixedImage)
  {
	reportError("The fixed data is not set, cannot do landmark image registration!");
    return;
  }

  //check that the moving data is set
  DataPtr movingImage = this->getMovingData();
  if(!movingImage)
  {
	reportError("The moving data is not set, cannot do landmark image registration!");
    return;
  }

  // ignore self-registration, this gives no effect bestcase, buggy behaviour worstcase (has been observed)
  if(movingImage==fixedImage)
  {
    reportError("The moving and fixed are equal, ignoring landmark image registration!");
    return;
  }

  LandmarkMap fixedLandmarks = fixedImage->getLandmarks()->getLandmarks();
  LandmarkMap imageLandmarks = movingImage->getLandmarks()->getLandmarks();

  this->writePreLandmarkRegistration(fixedImage->getName(), fixedImage->getLandmarks()->getLandmarks());
  this->writePreLandmarkRegistration(movingImage->getName(), movingImage->getLandmarks()->getLandmarks());

  std::vector<QString> landmarks = getUsableLandmarks(fixedLandmarks, imageLandmarks);
  vtkPointsPtr p_fixed_r = convertTovtkPoints(landmarks, fixedLandmarks, fixedImage->get_rMd());
  vtkPointsPtr p_moving_r = convertTovtkPoints(landmarks, imageLandmarks, movingImage->get_rMd());

  int minNumberOfPoints = 3;
  if (translationOnly)
	  minNumberOfPoints = 1;

  // ignore if too few data.
  if (p_fixed_r->GetNumberOfPoints() < minNumberOfPoints)
  {
    reportError(
    	QString("Found %1 corresponding landmarks, need %2, cannot do landmark image registration!")
    	.arg(p_fixed_r->GetNumberOfPoints())
    	.arg(minNumberOfPoints)
    	);
    return;
  }

  bool ok = false;
  QString idString;
  Transform3D delta;

  if (translationOnly)
  {
	  LandmarkTranslationRegistration landmarkTransReg;
	  delta = landmarkTransReg.registerPoints(convertVtkPointsToPoints(p_fixed_r), convertVtkPointsToPoints(p_moving_r), &ok);
	  idString = QString("Image to Image Landmark Translation");
  }
  else
  {
	  Transform3D rMd;
	  delta = this->performLandmarkRegistration(p_moving_r, p_fixed_r, &ok);
	  idString = QString("Image to Image Landmark");
  }

  if (!ok)
  {
    reportError("I-I Landmark registration: Failed to register: [" + qstring_cast(p_moving_r->GetNumberOfPoints()) + "p], "+ movingImage->getName());
    return;
  }

  this->applyImage2ImageRegistration(delta, idString);
}

/**Perform a fast orientation by setting the patient registration equal to the current dominant
 * tool position.
 * Input is an additional transform tMtm that modifies the tool position. Use this to
 * define DICOM-ish spaces relative to the tool.
 *
 */
void RegistrationManager::doFastRegistration_Orientation(const Transform3D& tMtm)
{
//  Transform3D rMpr = toolManager()->get_rMpr();
  Transform3D prMt = toolManager()->getDominantTool()->get_prMt();

  //create a marked(m) space tm, which is related to tool space (t) as follows:
  //the tool is defined in DICOM space such that
  //the tool points toward the patients feet and the spheres faces the same
  //direction as the nose
    Transform3D tMpr = prMt.inv();

  Transform3D tmMpr = tMtm * tMpr;

  this->applyPatientRegistration(tmMpr, "Fast Orientation");

  // also apply the fast translation registration if any (this frees us form doing stuff in a well-defined order.)
  this->doFastRegistration_Translation();
}

void RegistrationManager::doFastRegistration_Translation()
{
  DataPtr fixedImage = this->getFixedData();
  if(!fixedImage)
  {
	reportError("The fixed data is not set, cannot do image registration!");
    return;
  }

  LandmarkMap fixedLandmarks = fixedImage->getLandmarks()->getLandmarks();
  LandmarkMap toolLandmarks = dataManager()->getPatientLandmarks()->getLandmarks();

  this->writePreLandmarkRegistration(fixedImage->getName(), fixedImage->getLandmarks()->getLandmarks());
  this->writePreLandmarkRegistration("physical", toolLandmarks);

  std::vector<QString> landmarks = this->getUsableLandmarks(fixedLandmarks, toolLandmarks);

  Transform3D rMd = fixedImage->get_rMd();
  Transform3D rMpr_old = dataManager()->get_rMpr();
  std::vector<Vector3D> p_pr_old = this->convertAndTransformToPoints(landmarks, fixedLandmarks, rMpr_old.inv()*rMd);
  std::vector<Vector3D> p_pr_new = this->convertAndTransformToPoints(landmarks, toolLandmarks, Transform3D::Identity());

  // ignore if too few data.
  if (p_pr_old.size() < 1)
    return;

  LandmarkTranslationRegistration landmarkTransReg;
  bool ok = false;
  Transform3D pr_oldMpr_new = landmarkTransReg.registerPoints(p_pr_old, p_pr_new, &ok);
  if (!ok)
  {
    reportError("Fast translation registration: Failed to register: [" + qstring_cast(p_pr_old.size()) + "points]");
    return;
  }

  this->applyPatientRegistration(rMpr_old*pr_oldMpr_new, "Fast Translation");
}

/**\brief Identical to doFastRegistration_Orientation(), except data does not move.
 *
 * When applying a new transform to the patient orientation, all data is moved
 * the the inverse of that value, thus giving a net zero change along the path
 * pr...d_i.
 *
 */
void RegistrationManager::applyPatientOrientation(const Transform3D& tMtm)
{
	Transform3D rMpr = dataManager()->get_rMpr();
	Transform3D prMt = toolManager()->getDominantTool()->get_prMt();

	//create a marked(m) space tm, which is related to tool space (t) as follows:
	//the tool is defined in DICOM space such that
	//the tool points toward the patients feet and the spheres faces the same
	//direction as the nose
	Transform3D tMpr = prMt.inv();

	// this is the new patient registration:
	Transform3D tmMpr = tMtm * tMpr;
	// the change in pat reg becomes:
	Transform3D F = tmMpr * rMpr.inv();

	QString description("Patient Orientation");

	QDateTime oldTime = mRegistrationService->getLastRegistrationTime(); // time of previous reg
	this->applyPatientRegistration(tmMpr, description);

	// now apply the inverse of F to all data,
	// thus ensuring the total path from pr to d_i is unchanged:
	Transform3D delta_pre_rMd = F;


	// use the same registration time as generated in the applyPatientRegistration() above:
	RegistrationTransform regTrans(delta_pre_rMd, mRegistrationService->getLastRegistrationTime(), description);

	std::map<QString,DataPtr> data = dataManager()->getData();
	// update the transform on all target data:
	for (std::map<QString,DataPtr>::iterator iter = data.begin(); iter!=data.end(); ++iter)
	{
		DataPtr current = iter->second;
		RegistrationTransform newTransform = regTrans;
		newTransform.mValue = regTrans.mValue * current->get_rMd();
		current->get_rMd_History()->updateRegistration(oldTime, newTransform);

		report("Updated registration of data " + current->getName());
		std::cout << "rMd_new\n" << newTransform.mValue << std::endl;
	}

	mRegistrationService->setLastRegistrationTime(regTrans.mTimestamp);

	reportSuccess("Patient Orientation has been performed");
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
