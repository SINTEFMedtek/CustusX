/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxRegistrationImplService.h"

#include <ctkPluginContext.h>
#include <ctkServiceTracker.h>
#include <vtkPoints.h>
#include <vtkLandmarkTransform.h>
#include <vtkMatrix4x4.h>

#include "cxData.h"
#include "cxTypeConversions.h"
#include "cxLogger.h"
#include "cxRegistrationTransform.h"
#include "cxFrameForest.h"
#include "cxPatientModelService.h"
#include "cxRegistrationApplicator.h"
#include "cxLandmark.h"
#include "cxPatientModelServiceProxy.h"
#include "cxLandmarkTranslationRegistration.h"
#include "cxSessionStorageServiceProxy.h"
#include "cxXMLNodeWrapper.h"

namespace cx
{

RegistrationImplService::RegistrationImplService(ctkPluginContext *context) :
	mLastRegistrationTime(QDateTime::currentDateTime()),
	mContext(context),
	mPatientModelService(new PatientModelServiceProxy(context)),
	mSession(SessionStorageServiceProxy::create(context))
{
	connect(mSession.get(), &SessionStorageService::cleared, this, &RegistrationImplService::clearSlot);
    connect(mSession.get(), &SessionStorageService::isLoadingSecond, this, &RegistrationImplService::duringLoadPatientSlot);
	connect(mSession.get(), &SessionStorageService::isSaving, this, &RegistrationImplService::duringSavePatientSlot);
}

RegistrationImplService::~RegistrationImplService()
{
}

void RegistrationImplService::duringSavePatientSlot(QDomElement& node)
{
	XMLNodeAdder root(node);
	QDomElement managerNode = root.descend("managers").node().toElement();
	this->addXml(managerNode);
}

void RegistrationImplService::duringLoadPatientSlot(QDomElement& node)
{
	XMLNodeParser root(node);
	QDomElement registrationManager = root.descend("managers/registrationManager").node().toElement();
	if (!registrationManager.isNull())
		this->parseXml(registrationManager);
}

void RegistrationImplService::addXml(QDomNode& parentNode)
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

void RegistrationImplService::parseXml(QDomNode& dataNode)
{
	QString fixedData = dataNode.namedItem("fixedDataUid").toElement().text();
	this->setFixedData(fixedData);

	QString movingData = dataNode.namedItem("movingDataUid").toElement().text();
	this->setMovingData(movingData);
}

void RegistrationImplService::clearSlot()
{
	this->setLastRegistrationTime(QDateTime());
	this->setFixedData(DataPtr());
}

void RegistrationImplService::setMovingData(DataPtr data)
{
	this->setMovingData((data) ? data->getUid() : "");
}

void RegistrationImplService::setFixedData(DataPtr data)
{
	this->setFixedData((data) ? data->getUid() : "");
}

void RegistrationImplService::setMovingData(QString uid)
{
	if (uid==mMovingData)
		return;
	mMovingData = uid;
	emit movingDataChanged(mMovingData);
}

void RegistrationImplService::setFixedData(QString uid)
{
	if (uid==mFixedData)
		return;
	mFixedData = uid;
	emit fixedDataChanged(mFixedData);
}

DataPtr RegistrationImplService::getMovingData()
{
	return mPatientModelService->getData(mMovingData);
}

DataPtr RegistrationImplService::getFixedData()
{
	return mPatientModelService->getData(mFixedData);
}

QDateTime RegistrationImplService::getLastRegistrationTime()
{
	return mLastRegistrationTime;
}

void RegistrationImplService::setLastRegistrationTime(QDateTime time)
{
	mLastRegistrationTime = time;
}

void RegistrationImplService::doPatientRegistration()
{
	DataPtr fixedImage = this->getFixedData();

	if(!fixedImage)
	{
	reportError("The fixed data is not set, cannot do patient registration!");
		return;
	}
	LandmarkMap fixedLandmarks = fixedImage->getLandmarks()->getLandmarks();
	LandmarkMap toolLandmarks = mPatientModelService->getPatientLandmarks()->getLandmarks();

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

	this->addPatientRegistration(rMpr, "Patient Landmark");
}

void RegistrationImplService::doFastRegistration_Translation()
{
	DataPtr fixedImage = this->getFixedData();
	if(!fixedImage)
	{
	reportError("The fixed data is not set, cannot do image registration!");
		return;
	}

	LandmarkMap fixedLandmarks = fixedImage->getLandmarks()->getLandmarks();
	LandmarkMap toolLandmarks = mPatientModelService->getPatientLandmarks()->getLandmarks();

	this->writePreLandmarkRegistration(fixedImage->getName(), fixedImage->getLandmarks()->getLandmarks());
	this->writePreLandmarkRegistration("physical", toolLandmarks);

	std::vector<QString> landmarks = this->getUsableLandmarks(fixedLandmarks, toolLandmarks);

	Transform3D rMd = fixedImage->get_rMd();
	Transform3D rMpr_old = mPatientModelService->get_rMpr();
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

	this->addPatientRegistration(rMpr_old*pr_oldMpr_new, "Fast Translation");
}

/**Perform a fast orientation by setting the patient registration equal to the current active
 * tool position.
 * Input is an additional transform tMtm that modifies the tool position. Use this to
 * define DICOM-ish spaces relative to the tool.
 *
 */
void RegistrationImplService::doFastRegistration_Orientation(const Transform3D& tMtm, const Transform3D& prMt)
{
	//create a marked(m) space tm, which is related to tool space (t) as follows:
	//the tool is defined in DICOM space such that
	//the tool points toward the patients feet and the spheres faces the same
	//direction as the nose
		Transform3D tMpr = prMt.inv();

	Transform3D tmMpr = tMtm * tMpr;

	this->addPatientRegistration(tmMpr, "Fast Orientation");

	// also apply the fast translation registration if any (this frees us form doing stuff in a well-defined order.)
	this->doFastRegistration_Translation();
}

std::vector<Vector3D> RegistrationImplService::convertAndTransformToPoints(const std::vector<QString>& uids, const LandmarkMap& data, Transform3D M)
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

/**Inspect the landmarks in data a and b, find landmarks defined in both of them and
 * that also is active.
 * Return the uids of these landmarks.
 */
std::vector<QString> RegistrationImplService::getUsableLandmarks(const LandmarkMap& data_a, const LandmarkMap& data_b)
{
	std::vector<QString> retval;
	std::map<QString, LandmarkProperty> props = mPatientModelService->getLandmarkProperties();
	std::map<QString, LandmarkProperty>::iterator iter;

	for (iter=props.begin(); iter!=props.end(); ++iter)
	{
		QString uid = iter->first;
		if (data_a.count(uid) && data_b.count(uid) && iter->second.getActive())
			retval.push_back(uid);
	}
	return retval;
}

void RegistrationImplService::doImageRegistration(bool translationOnly)
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
		delta = this->performLandmarkRegistration(p_moving_r, p_fixed_r, &ok);
		idString = QString("Image to Image Landmark");
	}

	if (!ok)
	{
		reportError("I-I Landmark registration: Failed to register: [" + qstring_cast(p_moving_r->GetNumberOfPoints()) + "p], "+ movingImage->getName());
		return;
	}

	this->addImage2ImageRegistration(delta, idString);
}

std::vector<Vector3D> RegistrationImplService::convertVtkPointsToPoints(vtkPointsPtr base)
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
Transform3D RegistrationImplService::performLandmarkRegistration(vtkPointsPtr source, vtkPointsPtr target, bool* ok) const
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

void RegistrationImplService::addImage2ImageRegistration(Transform3D dMd, QString description)
{
	this->performImage2ImageRegistration(dMd, description);
}

void RegistrationImplService::updateImage2ImageRegistration(Transform3D dMd, QString description)
{
	this->performImage2ImageRegistration(dMd, description, true);
}

void RegistrationImplService::performImage2ImageRegistration(Transform3D dMd, QString description, bool temporaryRegistration)
{
	RegistrationTransform regTrans(dMd, QDateTime::currentDateTime(), description, temporaryRegistration);
	regTrans.mFixed = mFixedData;
	regTrans.mMoving = mMovingData;

	this->updateRegistration_rMd(mLastRegistrationTime, regTrans, this->getMovingData());

	mLastRegistrationTime = regTrans.mTimestamp;
	if(!temporaryRegistration)
		reportSuccess(QString("Image registration [%1] has been performed on %2").arg(description).arg(regTrans.mMoving) );
}

void RegistrationImplService::addPatientRegistration(Transform3D rMpr_new, QString description)
{
	this->performPatientRegistration(rMpr_new, description);
}

void RegistrationImplService::updatePatientRegistration(Transform3D rMpr_new, QString description)
{
	this->performPatientRegistration(rMpr_new, description, true);
}

void RegistrationImplService::performPatientRegistration(Transform3D rMpr_new, QString description, bool temporaryRegistration)
{
	RegistrationTransform regTrans(rMpr_new, QDateTime::currentDateTime(), description, temporaryRegistration);
	regTrans.mFixed = mFixedData;

	mPatientModelService->updateRegistration_rMpr(mLastRegistrationTime, regTrans);

	mLastRegistrationTime = regTrans.mTimestamp;
	if(!temporaryRegistration)
		reportSuccess(QString("Patient registration [%1] has been performed.").arg(description));
}

/** Update the registration for data and all data connected to its space.
 *
 * Registration is done relative to masterFrame, i.e. data is moved relative to the masterFrame.
 *
 */
void RegistrationImplService::updateRegistration_rMd(QDateTime oldTime, RegistrationTransform dMd, DataPtr data)
{
	RegistrationApplicator applicator(mPatientModelService->getDatas());
	dMd.mMoving = data->getUid();
	applicator.updateRegistration(oldTime, dMd);

	bool silent = dMd.mTemp;
	if(!silent)
		mPatientModelService->autoSave();
}

/**\brief Identical to doFastRegistration_Orientation(), except data does not move.
 *
 * When applying a new transform to the patient orientation, all data is moved
 * the the inverse of that value, thus giving a net zero change along the path
 * pr...d_i.
 *
 */
void RegistrationImplService::applyPatientOrientation(const Transform3D& tMtm, const Transform3D& prMt)
{
	Transform3D rMpr = mPatientModelService->get_rMpr();

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

	QDateTime oldTime = this->getLastRegistrationTime(); // time of previous reg
	this->addPatientRegistration(tmMpr, description);

	// now apply the inverse of F to all data,
	// thus ensuring the total path from pr to d_i is unchanged:
	Transform3D delta_pre_rMd = F;


	// use the same registration time as generated in the applyPatientRegistration() above:
	RegistrationTransform regTrans(delta_pre_rMd, this->getLastRegistrationTime(), description);

	std::map<QString,DataPtr> data = mPatientModelService->getDatas();
	// update the transform on all target data:
	for (std::map<QString,DataPtr>::iterator iter = data.begin(); iter!=data.end(); ++iter)
	{
		DataPtr current = iter->second;
		RegistrationTransform newTransform = regTrans;
		newTransform.mValue = regTrans.mValue * current->get_rMd();
		current->get_rMd_History()->addOrUpdateRegistration(oldTime, newTransform);

		report("Updated registration of data " + current->getName());
		std::cout << "rMd_new\n" << newTransform.mValue << std::endl;
	}

	this->setLastRegistrationTime(regTrans.mTimestamp);

	reportSuccess("Patient Orientation has been performed");
}

void RegistrationImplService::writePreLandmarkRegistration(QString name, LandmarkMap landmarks)
{
	QStringList lm;
	for (LandmarkMap::iterator iter=landmarks.begin(); iter!=landmarks.end(); ++iter)
	{
		lm << mPatientModelService->getLandmarkProperties()[iter->second.getUid()].getName();
	}

	QString msg = QString("Preparing to register [%1] containing the landmarks: [%2]").arg(name).arg(lm.join(","));
	report(msg);
}

/**Convert the landmarks given by uids to vtk points.
 * The coordinates are given by the input data,
 * and should be transformed by M.
 *
 * Prerequisite: all uids exist in data.
 */
vtkPointsPtr RegistrationImplService::convertTovtkPoints(const std::vector<QString>& uids, const LandmarkMap& data, Transform3D M)
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

bool RegistrationImplService::isNull()
{
	return false;
}

} /* namespace cx */
