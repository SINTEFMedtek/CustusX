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

namespace cx
{
RegistrationManager* RegistrationManager::mCxInstance = NULL;
RegistrationManager* registrationManager() { return RegistrationManager::getInstance(); }
RegistrationManager* RegistrationManager::getInstance()
{
  if (mCxInstance == NULL)
  {
    mCxInstance = new RegistrationManager();
  }
  return mCxInstance;
}

void RegistrationManager::shutdown()
{
  delete mCxInstance;
  mCxInstance = NULL;
}

RegistrationManager::RegistrationManager()
{}

RegistrationManager::~RegistrationManager()
{}

/** Start a new round of registrations : this collects fex adding several landmarks into one registration session.
 *
 */
void RegistrationManager::initialize()
{
  mLastRegistrationTime = QDateTime::currentDateTime();
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

// used for loading a rMpr directly from file. Not in use. Can be reused in user story intraoperative CT (kar)
void RegistrationManager::setManualPatientRegistration(ssc::Transform3D patientRegistration)
{
  ssc::messageManager()->sendWarning("RegistrationManager::setManualPatientRegistration NEEDS REFACTORING!!!");

  ssc::RegistrationTransform regTrans(patientRegistration, QDateTime::currentDateTime(), "Manual Patient");
  ssc::toolManager()->get_rMpr_History()->addRegistration(regTrans);

  //if an offset existed, its no longer valid and should be removed
  mPatientRegistrationOffset = ssc::Transform3D();

  ssc::messageManager()->sendInfo("Manual patient registration is set.");
}

void RegistrationManager::setManualPatientRegistrationOffsetSlot(ssc::Transform3D offset)
{
  ssc::Transform3D rMpr = *ssc::toolManager()->get_rMpr();
  ssc::Transform3D deltaOffset = offset*mPatientRegistrationOffset.inv();

  ssc::RegistrationTransform regTrans(deltaOffset*rMpr, QDateTime::currentDateTime(), "Manual Patient Offset");
  ssc::toolManager()->get_rMpr_History()->updateRegistration(mLastRegistrationTime, regTrans);
  mLastRegistrationTime = regTrans.mTimestamp;

  mPatientRegistrationOffset = offset;
}

ssc::Transform3D RegistrationManager::getManualPatientRegistrationOffset()
{
  return mPatientRegistrationOffset;
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
  FrameForest forest;
  QDomNode target = forest.getNode(qstring_cast(data->getUid()));
  QDomNode masterFrame = forest.getNode(masterFrameUid);
  QDomNode targetBase = forest.getOldestAncestorNotCommonToRef(target, masterFrame);
  std::vector<ssc::DataPtr> targetData = forest.getDataFromDescendantsAndSelf(targetBase);
  ssc::messageManager()->sendInfo("Update Registration using master " + masterFrameUid + " with delta matrix\n"+qstring_cast(deltaTransform.mValue));

  // update the transform on all target data:
  for (unsigned i=0; i<targetData.size(); ++i)
  {
    ssc::RegistrationTransform newTransform = deltaTransform;
    newTransform.mValue = deltaTransform.mValue * targetData[i]->get_rMd();
    targetData[i]->get_rMd_History()->updateRegistration(oldTime, newTransform);

    ssc::messageManager()->sendInfo("Updated registration of data " + targetData[i]->getName());
    //std::cout << "rMd_new\n" << newTransform.mValue << std::endl;
  }

//  skriv om update registration:
//  - ta inn enten transform3D eller parent frame + time, modifiser transformen men behold gamle data.
//  - sjekk ut alle setParentFrame (spesielt den nedenfor)

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

      if (targetData[i]->getParentFrame() == targetBaseUid)
      {
        ssc::messageManager()->sendInfo("Reset parent frame of " + targetData[i]->getName() + " to " + masterAncestorUid + ". targetbase=" + targetBaseUid);
//        //targetData[i]->setParentFrame(masterAncestorUid);
//        if (targetData[i]->get_rMd_History()->getData().empty())
//          return;
//        ssc::RegistrationTransform t = targetData[i]->get_rMd_History()->getData().back();
//        t.mParentFrame = masterAncestorUid;
//        void updateParentFrame(const QDateTime& oldTime, const ParentFrame& newTransform);

        targetData[i]->get_rMd_History()->updateParentFrame(oldTime, ssc::ParentFrame(masterAncestorUid, deltaTransform.mTimestamp, deltaTransform.mType));
      }
    }
  }
  // as we now have mutated the datamanager, forest is now outdated.

  FrameForest forest2;
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

/** Perform a landmark registration between the data sets source and target.
 *  Return transform from source to target.
 */
ssc::Transform3D RegistrationManager::performLandmarkRegistration(vtkPointsPtr source, vtkPointsPtr target, bool* ok) const
{
  *ok = false;


  // too few data samples: ignore
  if (source->GetNumberOfPoints() < 3)
  {
    return ssc::Transform3D();
  }

  vtkLandmarkTransformPtr landmarktransform = vtkLandmarkTransformPtr::New();
  landmarktransform->SetSourceLandmarks(source);
  landmarktransform->SetTargetLandmarks(target);
  landmarktransform->SetModeToRigidBody();
  source->Modified();
  target->Modified();
  landmarktransform->Update();

  ssc::Transform3D tar_M_src(landmarktransform->GetMatrix());

  if (QString::number(tar_M_src[0][0])=="nan") // harry but quick way to check badness of transform...
  {
    return ssc::Transform3D();
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

  std::vector<QString> landmarks = this->getUsableLandmarks(fixedLandmarks, toolLandmarks);

  vtkPointsPtr p_ref = this->convertTovtkPoints(landmarks, fixedLandmarks, fixedImage->get_rMd());
  vtkPointsPtr p_pr = this->convertTovtkPoints(landmarks, toolLandmarks, ssc::Transform3D());

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

  ssc::RegistrationTransform regTrans(rMpr, QDateTime::currentDateTime(), "Patient");
  regTrans.mFixed = mFixedData->getUid();
  ssc::toolManager()->get_rMpr_History()->updateRegistration(mLastRegistrationTime, regTrans);
  mLastRegistrationTime = regTrans.mTimestamp;

  mPatientRegistrationOffset = ssc::Transform3D();

  emit patientRegistrationPerformed();
  ssc::messageManager()->sendSuccess("Patient registration has been performed.");
}

void RegistrationManager::doImageRegistration(ssc::ImagePtr image)
{
  //check that the fixed data is set
  if(!mFixedData)
  {
    return;
  }

  // ignore self-registration, this gives no effect bestcase, buggy behaviour worstcase (has been observed)
  if(image==boost::shared_dynamic_cast<ssc::Image>(mFixedData))
    return;

  ssc::ImagePtr fixedImage = boost::dynamic_pointer_cast<ssc::Image>(mFixedData);
  if(!fixedImage)
  {
    ssc::messageManager()->sendError("The fixed data is not a image, cannot do image registration!");
    return;
  }

  ssc::LandmarkMap fixedLandmarks = fixedImage->getLandmarks();
  ssc::LandmarkMap imageLandmarks = image->getLandmarks();

  std::vector<QString> landmarks = getUsableLandmarks(fixedLandmarks, imageLandmarks);
  vtkPointsPtr p_ref = convertTovtkPoints(landmarks, fixedLandmarks, fixedImage->get_rMd());
  vtkPointsPtr p_data = convertTovtkPoints(landmarks, imageLandmarks, ssc::Transform3D());

  if (landmarks.empty())
    return;
  // ignore if too few data.
  if (p_ref->GetNumberOfPoints() < 3)
    return;

  bool ok = false;
  ssc::Transform3D rMd = this->performLandmarkRegistration(p_data, p_ref, &ok);
  if (!ok)
  {
    ssc::messageManager()->sendError("I-I Landmark registration: Failed to register: [" + qstring_cast(p_data->GetNumberOfPoints()) + "p], "+ image->getName());
    return;
  }

  ssc::Transform3D delta = rMd * image->get_rMd().inv();

  ssc::RegistrationTransform regTrans(delta, QDateTime::currentDateTime(), "Image to Image");
  regTrans.mFixed = mFixedData->getUid();
  regTrans.mMoving = image->getUid();
  this->updateRegistration(mLastRegistrationTime, regTrans, image, qstring_cast(fixedImage->getUid()));

  mLastRegistrationTime = regTrans.mTimestamp;

  //emit imageRegistrationPerformed();
  ssc::messageManager()->sendSuccess("Image registration has been performed for " + image->getName());
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

  ssc::RegistrationTransform regTrans(tmMpr, QDateTime::currentDateTime(), "Fast_Orientation");
  ssc::toolManager()->get_rMpr_History()->updateRegistration(mLastRegistrationTime, regTrans);
  mLastRegistrationTime = regTrans.mTimestamp;

  ssc::messageManager()->sendSuccess("Fast orientation registration has been performed.");

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

  std::vector<QString> landmarks = this->getUsableLandmarks(fixedLandmarks, toolLandmarks);

  ssc::Transform3D rMd = fixedImage->get_rMd();
  ssc::Transform3D rMpr_old = *ssc::toolManager()->get_rMpr();
  std::vector<ssc::Vector3D> p_pr_old = this->convertAndTransformToPoints(landmarks, fixedLandmarks, rMpr_old.inv()*rMd);
  std::vector<ssc::Vector3D> p_pr_new = this->convertAndTransformToPoints(landmarks, toolLandmarks, ssc::Transform3D());

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

  ssc::RegistrationTransform regTrans(rMpr_old*pr_oldMpr_new, QDateTime::currentDateTime(), "Fast_Translation");
  regTrans.mFixed = mFixedData->getUid();
  ssc::toolManager()->get_rMpr_History()->updateRegistration(mLastRegistrationTime, regTrans);
  mLastRegistrationTime = regTrans.mTimestamp;

  mPatientRegistrationOffset = ssc::Transform3D();

  //emit fastRegistrationPerformed();
  ssc::messageManager()->sendSuccess("Fast translation registration has been performed.");
}

void RegistrationManager::doVesselRegistration(int lts_ratio, double stop_delta, double lambda, double sigma, bool lin_flag, int sample, int single_point_thre, bool verbose)
{

  //Default values
  /*int lts_ratio = 80;
  double stop_delta = 0.001;
  double lambda = 0;
  double sigma = 1.0;
  bool lin_flag = 1;
  int sample = 1;
  int single_point_thre = 1;
  bool verbose = 1;*/

  SeansVesselReg vesselReg(lts_ratio,
        stop_delta,
        lambda,
        sigma,
        lin_flag,
        sample,
        single_point_thre,
        verbose);

  ssc::ImagePtr fixedData = boost::dynamic_pointer_cast<ssc::Image>(mFixedData);
  ssc::ImagePtr movingData = boost::dynamic_pointer_cast<ssc::Image>(mMovingData);

  if(!fixedData)
  {
    ssc::messageManager()->sendError("Could not cast fixeddata to image.");
    return;
  }
  if(!movingData)
  {
    ssc::messageManager()->sendError("Could not cast moving data to image.");
    return;
  }

  bool success = vesselReg.doItRight(fixedData, movingData);
  if(!success)
  {
    ssc::messageManager()->sendWarning("Vessel registration failed.");
    return;
  }

  ssc::Transform3D linearTransform = vesselReg.getLinearTransform();
  std::cout << "v2v linear result:\n" << linearTransform << std::endl;

//  ssc::Transform3D delta = fixedData->get_rMd() * linearTransform * movingData->get_rMd().inv();

  // The registration is performed in space r. Thus, given an old data position rMd, we find the
  // new one as rM'd = Q * rMd, where Q is the inverted registration output.
  // Delta is thus equal to Q:
  ssc::Transform3D delta = linearTransform.inv();
  //std::cout << "delta:\n" << delta << std::endl;
  ssc::RegistrationTransform regTrans(delta, QDateTime::currentDateTime(), "Vessel based");
  regTrans.mFixed = mFixedData->getUid();
  regTrans.mMoving = mMovingData->getUid();
  this->updateRegistration(mLastRegistrationTime, regTrans, movingData, qstring_cast(fixedData->getUid()));

  ssc::messageManager()->sendSuccess("Vessel based registration has been performed.");

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

//
//  QDomNode child = dataNode.firstChild();
//  while(!child.isNull())
//  {
//    if(child.toElement().tagName() == "fixedDataUid")
//    {
//      const QString fixedDataString = child.toElement().text();
//      if(!fixedDataString.isEmpty())
//      {
//        ssc::DataPtr data = ssc::dataManager()->getData(fixedDataString);
//        this->setFixedData(data);
//      }
//    }
//    child = child.nextSibling();
//  }
}

void RegistrationManager::clear()
{
  mLastRegistrationTime = QDateTime();
  this->setFixedData(ssc::DataPtr());
}

}//namespace cx
