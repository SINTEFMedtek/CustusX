#include "cxRegistrationManager.h"

#include <QtCore>
#include <QDomElement>
#include "vtkMath.h"
#include "vtkPoints.h"
#include "vtkDoubleArray.h"
#include "vtkLandmarkTransform.h"
#include "vtkMatrix4x4.h"
#include "sscTransform3D.h"
#include "sscRegistrationTransform.h"
#include "sscMessageManager.h"
#include "sscToolManager.h"
#include "sscDataManager.h"
#include "cxLandmarkTranslationRegistration.h"
#include "cxFrameForest.h"
#include "vesselReg/SeansVesselReg.hxx"
#include "cxPatientService.h"
#include "cxPatientData.h"

namespace cx
{


RegistrationManager::RegistrationManager(AcquisitionDataPtr acquisitionData) :
		mAcquisitionData(acquisitionData)
{
	this->restart();
  connect(patientService()->getPatientData().get(), SIGNAL(isSaving()), this, SLOT(duringSavePatientSlot()));
  connect(patientService()->getPatientData().get(), SIGNAL(isLoading()), this, SLOT(duringLoadPatientSlot()));
  connect(patientService()->getPatientData().get(), SIGNAL(cleared()), this, SLOT(clearSlot()));
}

void RegistrationManager::restart()
{
//  mPatientRegistrationOffset = ssc::Transform3D::Identity();
  mLastRegistrationTime = QDateTime::currentDateTime();
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

void RegistrationManager::setFixedData(ssc::DataPtr fixedData)
{
  if(boost::dynamic_pointer_cast<ssc::Image>(mFixedData) == fixedData)
    return;

  mFixedData = fixedData;
  if (mFixedData)
    ssc::messageManager()->sendInfo("Registration fixed data set to "+mFixedData->getUid());
  emit fixedDataChanged( (mFixedData) ? qstring_cast(mFixedData->getUid()) : "");
}

ssc::DataPtr RegistrationManager::getFixedData()
{
  return mFixedData;
}

void RegistrationManager::setMovingData(ssc::DataPtr movingData)
{
  mMovingData = movingData;
  emit movingDataChanged( (mMovingData) ? qstring_cast(mMovingData->getUid()) : "");
}

ssc::DataPtr RegistrationManager::getMovingData()
{
  return mMovingData;
}


/**Inspect the landmarks in data a and b, find landmarks defined in both of them and
 * that also is active.
 * Return the uids of these landmarks.
 */
std::vector<QString> RegistrationManager::getUsableLandmarks(const ssc::LandmarkMap& data_a, const ssc::LandmarkMap& data_b)
{
  std::vector<QString> retval;
  std::map<QString, ssc::LandmarkProperty> props = ssc::dataManager()->getLandmarkProperties();
  std::map<QString, ssc::LandmarkProperty>::iterator iter;

  for (iter=props.begin(); iter!=props.end(); ++iter)
  {
    QString uid = iter->first;
    if (data_a.count(uid) && data_b.count(uid) && iter->second.getActive())
      retval.push_back(uid);
  }
  return retval;
}

/** Update the registration for data and all data connected to its space.
 *
 * Registration is done relative to masterFrame, i.e. data is moved relative to the masterFrame.
 *
 */
void RegistrationManager::updateRegistration(QDateTime oldTime, ssc::RegistrationTransform deltaTransform, ssc::DataPtr data, QString masterFrameUid)
{
//	std::cout << "==== RegistrationManager::updateRegistration" << std::endl;
  FrameForest forest;
  QDomNode target = forest.getNode(qstring_cast(data->getUid()));
  QDomNode masterFrame = target;
  QDomNode targetBase = target;
  if (masterFrameUid!="")
  {
	  masterFrame = forest.getNode(masterFrameUid);
	  targetBase = forest.getOldestAncestorNotCommonToRef(target, masterFrame);
  }
  std::vector<ssc::DataPtr> targetData = forest.getDataFromDescendantsAndSelf(targetBase);

  std::stringstream ss;
  ss << "Update Registration using " << std::endl;
  ss << "\tFixed:\t" << masterFrameUid << std::endl;
  ss << "\tMoving:\t" << data->getUid() << std::endl;
  ss << "\tDelta matrix (rMd'=Delta*rMd)\n"+qstring_cast(deltaTransform.mValue) << std::endl;
  ssc::messageManager()->sendInfo(qstring_cast(ss.str()));

  // update the transform on all target data:
  for (unsigned i=0; i<targetData.size(); ++i)
  {
    ssc::RegistrationTransform newTransform = deltaTransform;
    newTransform.mValue = deltaTransform.mValue * targetData[i]->get_rMd();
    targetData[i]->get_rMd_History()->updateRegistration(oldTime, newTransform);

    ssc::messageManager()->sendInfo("Updated registration of data " + targetData[i]->getName());
    //std::cout << "rMd_new\n" << newTransform.mValue << std::endl; // too much noise for large patients
  }

//  skriv om update registration:
//  - ta inn enten transform3D eller parent frame + time, modifiser transformen men behold gamle data.
//  - sjekk ut alle setParentSpace (spesielt den nedenfor)

  //error:
  // reconnect only if the registration is done relative to a base.
  // if target==targetBase, the registration is done inside an already connected
  // tree and we dont need (or want - leads to error) to reconnect.
  //if (target!=targetBase)

  // reconnect only if master and target are unconnected, i.e. share a common ancestor.
  // If we are registrating inside an already connected tree we only want to change transforms,
  // not change the topology of the tree.
  if (forest.getOldestAncestor(target) != forest.getOldestAncestor(masterFrame))
  {
    // connect the target to the master's ancestor, i.e. replace targetBase with masterAncestor:
    QDomNode masterAncestor = forest.getOldestAncestor(masterFrame);
    // iterate over all target data,
    //forest.reconnectFrame(targetBase, masterAncestor); // alternative if we move operation below into forest
    for (unsigned i=0; i<targetData.size(); ++i)
    {
      QString masterAncestorUid = masterAncestor.toElement().tagName();
      QString targetBaseUid = targetBase.toElement().tagName();

      if (targetData[i]->getParentSpace() == targetBaseUid)
      {
        ssc::messageManager()->sendInfo("Reset parent frame of " + targetData[i]->getName() + " to " + masterAncestorUid + ". targetbase=" + targetBaseUid);
//        //targetData[i]->setParentSpace(masterAncestorUid);
//        if (targetData[i]->get_rMd_History()->getData().empty())
//          return;
//        ssc::RegistrationTransform t = targetData[i]->get_rMd_History()->getData().back();
//        t.mParentFrame = masterAncestorUid;
//        void updateParentSpace(const QDateTime& oldTime, const ParentSpace& newTransform);

        targetData[i]->get_rMd_History()->updateParentSpace(oldTime, ssc::ParentSpace(masterAncestorUid, deltaTransform.mTimestamp, deltaTransform.mType));
      }
    }
  }
  // as we now have mutated the datamanager, forest is now outdated.

  FrameForest forest2;
//	std::cout << "    ==== RegistrationManager::updateRegistration" << std::endl;
}

/**Convert the landmarks given by uids to vtk points.
 * The coordinates are given by the input data,
 * and should be transformed by M.
 *
 * Prerequisite: all uids exist in data.
 */
vtkPointsPtr RegistrationManager::convertTovtkPoints(const std::vector<QString>& uids, const ssc::LandmarkMap& data, ssc::Transform3D M)
{
  vtkPointsPtr retval = vtkPointsPtr::New();

  for (unsigned i=0; i<uids.size(); ++i)
  {
    QString uid = uids[i];
    ssc::Vector3D p = M.coord(data.find(uid)->second.getCoord());
    retval->InsertNextPoint(p.begin());
  }
  return retval;
}

std::vector<ssc::Vector3D> RegistrationManager::convertAndTransformToPoints(const std::vector<QString>& uids, const ssc::LandmarkMap& data, ssc::Transform3D M)
{
  std::vector<ssc::Vector3D> retval;

  for (unsigned i=0; i<uids.size(); ++i)
  {
    QString uid = uids[i];
    ssc::Vector3D p = M.coord(data.find(uid)->second.getCoord());
    retval.push_back(p);
  }
  return retval;
}

std::vector<ssc::Vector3D> RegistrationManager::convertVtkPointsToPoints(vtkPointsPtr base)
{
  std::vector<ssc::Vector3D> retval;

  for (int i=0; i<base->GetNumberOfPoints(); ++i)
  {
    ssc::Vector3D p(base->GetPoint(i));
    retval.push_back(p);
  }
  return retval;
}

/** Perform a landmark registration between the data sets source and target.
 *  Return transform from source to target.
 */
ssc::Transform3D RegistrationManager::performLandmarkRegistration(vtkPointsPtr source, vtkPointsPtr target, bool* ok) const
{
  *ok = false;

  // too few data samples: ignore
  if (source->GetNumberOfPoints() < 3)
  {
    return ssc::Transform3D::Identity();
  }

  vtkLandmarkTransformPtr landmarktransform = vtkLandmarkTransformPtr::New();
  landmarktransform->SetSourceLandmarks(source);
  landmarktransform->SetTargetLandmarks(target);
  landmarktransform->SetModeToRigidBody();
  source->Modified();
  target->Modified();
  landmarktransform->Update();

  ssc::Transform3D tar_M_src(landmarktransform->GetMatrix());

  if (QString::number(tar_M_src(0,0))=="nan") // harry but quick way to check badness of transform...
  {
    return ssc::Transform3D::Identity();
  }

  *ok = true;
  return tar_M_src;
}

void RegistrationManager::doPatientRegistration()
{
  if(!mFixedData)
    return;

  ssc::ImagePtr fixedImage = boost::dynamic_pointer_cast<ssc::Image>(mFixedData);

  if(!fixedImage)
  {
    ssc::messageManager()->sendError("The fixed data is not a image, cannot do patient registration!");
    return;
  }
  ssc::LandmarkMap fixedLandmarks = fixedImage->getLandmarks();
  ssc::LandmarkMap toolLandmarks = ssc::toolManager()->getLandmarks();

  this->writePreLandmarkRegistration(fixedImage->getName(), fixedImage->getLandmarks());
  this->writePreLandmarkRegistration("physical", toolLandmarks);

  std::vector<QString> landmarks = this->getUsableLandmarks(fixedLandmarks, toolLandmarks);

  vtkPointsPtr p_ref = this->convertTovtkPoints(landmarks, fixedLandmarks, fixedImage->get_rMd());
  vtkPointsPtr p_pr = this->convertTovtkPoints(landmarks, toolLandmarks, ssc::Transform3D::Identity());

  // ignore if too few data.
  if (p_ref->GetNumberOfPoints() < 3)
    return;

  bool ok = false;
  ssc::Transform3D rMpr = this->performLandmarkRegistration(p_pr, p_ref, &ok);
  if (!ok)
  {
    ssc::messageManager()->sendError("P-I Landmark registration: Failed to register: [" + qstring_cast(p_pr->GetNumberOfPoints()) + "p]");
    return;
  }

  this->applyPatientRegistration(rMpr, "Patient Landmark");
}

void RegistrationManager::writePreLandmarkRegistration(QString name, ssc::LandmarkMap landmarks)
{
	QStringList lm;
	for (ssc::LandmarkMap::iterator iter=landmarks.begin(); iter!=landmarks.end(); ++iter)
	{
		lm << ssc::dataManager()->getLandmarkProperties()[iter->second.getUid()].getName();
	}

	QString msg = QString("Preparing to register [%1] containing the landmarks: [%2]").arg(name).arg(lm.join(","));
	ssc::messageManager()->sendInfo(msg);
}


void RegistrationManager::doImageRegistration(bool translationOnly)
{
  //check that the fixed data is set
  ssc::ImagePtr fixedImage = boost::dynamic_pointer_cast<ssc::Image>(mFixedData);
  if(!fixedImage)
  {
    ssc::messageManager()->sendError("The fixed data is not a image, cannot do landmark image registration!");
    return;
  }

  //check that the moving data is set
  ssc::ImagePtr movingImage = boost::dynamic_pointer_cast<ssc::Image>(mMovingData);
  if(!movingImage)
  {
    ssc::messageManager()->sendError("The moving data is not a image, cannot do landmark image registration!");
    return;
  }

  // ignore self-registration, this gives no effect bestcase, buggy behaviour worstcase (has been observed)
  if(movingImage==fixedImage)
  {
    ssc::messageManager()->sendError("The moving and fixed are equal, ignoring landmark image registration!");
    return;
  }

  ssc::LandmarkMap fixedLandmarks = fixedImage->getLandmarks();
  ssc::LandmarkMap imageLandmarks = movingImage->getLandmarks();

  this->writePreLandmarkRegistration(fixedImage->getName(), fixedImage->getLandmarks());
  this->writePreLandmarkRegistration(movingImage->getName(), movingImage->getLandmarks());

  std::vector<QString> landmarks = getUsableLandmarks(fixedLandmarks, imageLandmarks);
//  vtkPointsPtr p_ref = convertTovtkPoints(landmarks, fixedLandmarks, fixedImage->get_rMd());
//  vtkPointsPtr p_data = convertTovtkPoints(landmarks, imageLandmarks, ssc::Transform3D::Identity());
  vtkPointsPtr p_fixed_r = convertTovtkPoints(landmarks, fixedLandmarks, fixedImage->get_rMd());
  vtkPointsPtr p_moving_r = convertTovtkPoints(landmarks, imageLandmarks, movingImage->get_rMd());

  int minNumberOfPoints = 3;
  if (translationOnly)
	  minNumberOfPoints = 1;

  // ignore if too few data.
  if (p_fixed_r->GetNumberOfPoints() < minNumberOfPoints)
  {
    ssc::messageManager()->sendError(
    	QString("Found %1 corresponding landmarks, need %2, cannot do landmark image registration!")
    	.arg(p_fixed_r->GetNumberOfPoints())
    	.arg(minNumberOfPoints)
    	);
    return;
  }

  bool ok = false;
  QString idString;
  ssc::Transform3D delta;

  if (translationOnly)
  {
	  LandmarkTranslationRegistration landmarkTransReg;
	  delta = landmarkTransReg.registerPoints(convertVtkPointsToPoints(p_fixed_r), convertVtkPointsToPoints(p_moving_r), &ok);
	  idString = QString("Image to Image Landmark Translation");
  }
  else
  {
	  ssc::Transform3D rMd;
	  delta = this->performLandmarkRegistration(p_moving_r, p_fixed_r, &ok);
	  idString = QString("Image to Image Landmark");
  }

  if (!ok)
  {
    ssc::messageManager()->sendError("I-I Landmark registration: Failed to register: [" + qstring_cast(p_moving_r->GetNumberOfPoints()) + "p], "+ movingImage->getName());
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
void RegistrationManager::doFastRegistration_Orientation(const ssc::Transform3D& tMtm)
{
  ssc::Transform3DPtr rMpr = ssc::toolManager()->get_rMpr();
  ssc::Transform3D prMt = ssc::toolManager()->getDominantTool()->get_prMt();

  //create a marked(m) space tm, which is related to tool space (t) as follows:
  //the tool is defined in DICOM space such that
  //the tool points toward the patients feet and the spheres faces the same
  //direction as the nose
    ssc::Transform3D tMpr = prMt.inv();

  ssc::Transform3D tmMpr = tMtm * tMpr;

  this->applyPatientRegistration(tmMpr, "Fast Orientation");

  // also apply the fast translation registration if any (this frees us form doing stuff in a well-defined order.)
  this->doFastRegistration_Translation();
}

void RegistrationManager::doFastRegistration_Translation()
{
  if(!mFixedData)
    return;

  ssc::ImagePtr fixedImage = boost::dynamic_pointer_cast<ssc::Image>(mFixedData);
  if(!fixedImage)
  {
    ssc::messageManager()->sendError("The fixed data is not a image, cannot do image registration!");
    return;
  }

  ssc::LandmarkMap fixedLandmarks = fixedImage->getLandmarks();
  ssc::LandmarkMap toolLandmarks = ssc::toolManager()->getLandmarks();

  this->writePreLandmarkRegistration(fixedImage->getName(), fixedImage->getLandmarks());
  this->writePreLandmarkRegistration("physical", toolLandmarks);

  std::vector<QString> landmarks = this->getUsableLandmarks(fixedLandmarks, toolLandmarks);

  ssc::Transform3D rMd = fixedImage->get_rMd();
  ssc::Transform3D rMpr_old = *ssc::toolManager()->get_rMpr();
  std::vector<ssc::Vector3D> p_pr_old = this->convertAndTransformToPoints(landmarks, fixedLandmarks, rMpr_old.inv()*rMd);
  std::vector<ssc::Vector3D> p_pr_new = this->convertAndTransformToPoints(landmarks, toolLandmarks, ssc::Transform3D::Identity());

  // ignore if too few data.
  if (p_pr_old.size() < 1)
    return;

  LandmarkTranslationRegistration landmarkTransReg;
  bool ok = false;
  ssc::Transform3D pr_oldMpr_new = landmarkTransReg.registerPoints(p_pr_old, p_pr_new, &ok);
  if (!ok)
  {
    ssc::messageManager()->sendError("Fast translation registration: Failed to register: [" + qstring_cast(p_pr_old.size()) + "points]");
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
void RegistrationManager::applyPatientOrientation(const ssc::Transform3D& tMtm)
{
	ssc::Transform3D rMpr = *ssc::toolManager()->get_rMpr();
	ssc::Transform3D prMt = ssc::toolManager()->getDominantTool()->get_prMt();

	//create a marked(m) space tm, which is related to tool space (t) as follows:
	//the tool is defined in DICOM space such that
	//the tool points toward the patients feet and the spheres faces the same
	//direction as the nose
	ssc::Transform3D tMpr = prMt.inv();

	// this is the new patient registration:
	ssc::Transform3D tmMpr = tMtm * tMpr;
	// the change in pat reg becomes:
	ssc::Transform3D F = tmMpr * rMpr.inv();

	QString description("Patient Orientation");

	QDateTime oldTime = mLastRegistrationTime; // time of previous reg
	this->applyPatientRegistration(tmMpr, description);

	// now apply the inverse of F to all data,
	// thus ensuring the total path from pr to d_i is unchanged:
	ssc::Transform3D delta_pre_rMd = F;


	// use the same registration time as generated in the applyPatientRegistration() above:
	ssc::RegistrationTransform regTrans(delta_pre_rMd, mLastRegistrationTime, description);

	std::map<QString,ssc::DataPtr> data = ssc::dataManager()->getData();
	// update the transform on all target data:
	for (std::map<QString,ssc::DataPtr>::iterator iter = data.begin(); iter!=data.end(); ++iter)
	{
		ssc::DataPtr current = iter->second;
		ssc::RegistrationTransform newTransform = regTrans;
		newTransform.mValue = regTrans.mValue * current->get_rMd();
		current->get_rMd_History()->updateRegistration(oldTime, newTransform);

		ssc::messageManager()->sendInfo("Updated registration of data " + current->getName());
		std::cout << "rMd_new\n" << newTransform.mValue << std::endl;
	}

	mLastRegistrationTime = regTrans.mTimestamp;

	ssc::messageManager()->sendSuccess("Patient Orientation has been performed");
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
void RegistrationManager::applyImage2ImageRegistration(ssc::Transform3D delta_pre_rMd, QString description)
{
	ssc::RegistrationTransform regTrans(delta_pre_rMd, QDateTime::currentDateTime(), description);
	regTrans.mFixed = mFixedData ? mFixedData->getUid() : "";
	regTrans.mMoving = mMovingData ? mMovingData->getUid() : "";
	this->updateRegistration(mLastRegistrationTime, regTrans, mMovingData, regTrans.mFixed);
	mLastRegistrationTime = regTrans.mTimestamp;
	ssc::messageManager()->sendSuccess(QString("Image registration [%1] has been performed on %2").arg(description).arg(regTrans.mMoving) );
}

/**\brief apply a new patient registration
 *
 * All patient registration techniques should use this method
 * to apply the found patient registration to the system.
 *
 */
void RegistrationManager::applyPatientRegistration(ssc::Transform3D rMpr_new, QString description)
{
	ssc::RegistrationTransform regTrans(rMpr_new, QDateTime::currentDateTime(), description);
	regTrans.mFixed = mFixedData ? mFixedData->getUid() : "";
	ssc::toolManager()->get_rMpr_History()->updateRegistration(mLastRegistrationTime, regTrans);
	mLastRegistrationTime = regTrans.mTimestamp;
	ssc::messageManager()->sendSuccess(QString("Patient registration [%1] has been performed.").arg(description));
}

void RegistrationManager::addXml(QDomNode& parentNode)
{
  QDomDocument doc = parentNode.ownerDocument();
  QDomElement base = doc.createElement("registrationManager");
  parentNode.appendChild(base);

  QDomElement fixedDataNode = doc.createElement("fixedDataUid");
  if(mFixedData)
  {
    fixedDataNode.appendChild(doc.createTextNode(mFixedData->getUid()));
  }
  base.appendChild(fixedDataNode);

  QDomElement movingDataNode = doc.createElement("movingDataUid");
  if(mMovingData)
  {
    movingDataNode.appendChild(doc.createTextNode(mMovingData->getUid()));
  }
  base.appendChild(movingDataNode);
}

void RegistrationManager::parseXml(QDomNode& dataNode)
{
  QString fixedData = dataNode.namedItem("fixedDataUid").toElement().text();
  this->setFixedData(ssc::dataManager()->getData(fixedData));

  QString movingData = dataNode.namedItem("movingDataUid").toElement().text();
  this->setMovingData(ssc::dataManager()->getData(movingData));
}

void RegistrationManager::clearSlot()
{
  mLastRegistrationTime = QDateTime();
  this->setFixedData(ssc::DataPtr());
}

}//namespace cx
