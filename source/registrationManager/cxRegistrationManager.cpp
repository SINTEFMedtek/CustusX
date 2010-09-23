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

void RegistrationManager::setMasterImage(ssc::ImagePtr image)
{
  if (mMasterImage==image)
    return;

  mMasterImage = image;
  if (mMasterImage)
    ssc::messageManager()->sendInfo("Registration Master image set to "+image->getUid());
}

ssc::ImagePtr RegistrationManager::getMasterImage()
{
  return mMasterImage;
}

bool RegistrationManager::isMasterImageSet()
{
  return mMasterImage;
}

void RegistrationManager::setManualPatientRegistration(ssc::Transform3D patientRegistration)
{
  ssc::messageManager()->sendWarning("RegistrationManager::setManualPatientRegistration NEEDS REFACTORING!!!");

  //mManualPatientRegistration = patientRegistration;

  ssc::RegistrationTransform regTrans(patientRegistration, QDateTime::currentDateTime(), "Manual Patient");
  ssc::toolManager()->get_rMpr_History()->addRegistration(regTrans);

  //if an offset existed, its no longer valid and should be removed
  mPatientRegistrationOffset = ssc::Transform3D();

  ssc::messageManager()->sendInfo("Manual patient registration is set.");
}

//ssc::Transform3DPtr RegistrationManager::getManualPatientRegistration()
//{
//  return mManualPatientRegistration;
//}

//void RegistrationManager::resetManualPatientientRegistration()
//{
//  mManualPatientRegistration.reset();
//  this->doPatientRegistration();
//}

void RegistrationManager::setManualPatientRegistrationOffsetSlot(ssc::Transform3D offset)
{
  ssc::Transform3D rMpr = *ssc::toolManager()->get_rMpr();
  ssc::Transform3D deltaOffset = offset*mPatientRegistrationOffset.inv();

  ssc::RegistrationTransform regTrans(deltaOffset*rMpr, QDateTime::currentDateTime(), "Manual Patient Offset");
  ssc::toolManager()->get_rMpr_History()->updateRegistration(mLastRegistrationTime, regTrans);
  mLastRegistrationTime = regTrans.mTimestamp;

  mPatientRegistrationOffset = offset;
  ssc::messageManager()->sendInfo("Offset for the patient registration is set.");
}

ssc::Transform3D RegistrationManager::getManualPatientRegistrationOffset()
{
  return mPatientRegistrationOffset;
}

//void RegistrationManager::resetOffset()
//{
//  mPatientRegistrationOffset = ssc::Transform3D();
//  this->doPatientRegistration();
//}

/**Inspect the landmarks in data a and b, find landmarks defined in both of them and
 * that also is active.
 * Return the uids of these landmarks.
 */
std::vector<std::string> RegistrationManager::getUsableLandmarks(const ssc::LandmarkMap& data_a, const ssc::LandmarkMap& data_b)
{
  std::vector<std::string> retval;
  std::map<std::string, ssc::LandmarkProperty> props = ssc::dataManager()->getLandmarkProperties();
  std::map<std::string, ssc::LandmarkProperty>::iterator iter;

  for (iter=props.begin(); iter!=props.end(); ++iter)
  {
    std::string uid = iter->first;
    if (data_a.count(uid) && data_b.count(uid) && iter->second.getActive())
      retval.push_back(uid);
  }
  return retval;
}

//class FrameGraphNode
//{
//  std::vector<ssc::DataPtr> getAllSiblings()
//  {
//
//  }
//  QDomNode mNode;
//};


void RegistrationManager::updateRegistration(QDateTime oldTime, ssc::RegistrationTransform deltaTransform, ssc::DataPtr data)
{
//  std::string parentFrame = data->getParentFrame();
//
//  std::map<std::string, ssc::DataPtr> dataMap = ssc::dataManager()->getData();
//  std::map<std::string, ssc::DataPtr>::iterator it;
//  for(it = dataMap.begin(); it!=dataMap.end(); ++it)
//  {
//    if(frameOfReferenceUid == it->second->getFrameOfReferenceUid())
//    {
//      ssc::RegistrationTransform newTransform = deltaTransform;
//      newTransform.mValue = deltaTransform.mValue * it->second->get_rMd();
//      it->second->get_rMd_History()->updateRegistration(oldTime, deltaTransform);
//    }
//  }

}

/**Convert the landmarks given by uids to vtk points.
 * The coordinates are given by the input data,
 * and should be transformed by M.
 *
 * Prerequisite: all uids exist in data.
 */
vtkPointsPtr RegistrationManager::convertTovtkPoints(const std::vector<std::string>& uids, const ssc::LandmarkMap& data, ssc::Transform3D M)
{
  vtkPointsPtr retval = vtkPointsPtr::New();

  for (unsigned i=0; i<uids.size(); ++i)
  {
    std::string uid = uids[i];
    ssc::Vector3D p = M.coord(data.find(uid)->second.getCoord());
    retval->InsertNextPoint(p.begin());
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
    ssc::messageManager()->sendInfo("not enough points to register");
    return ssc::Transform3D();
  }

  vtkLandmarkTransformPtr landmarktransform = vtkLandmarkTransformPtr::New();
  landmarktransform->SetSourceLandmarks(source);
  landmarktransform->SetTargetLandmarks(target);
  //landmarktransform->SetModeToSimilarity(); // this allows scaling. Very dangerous!
  landmarktransform->SetModeToRigidBody();
  source->Modified();
  target->Modified();
  landmarktransform->Update();
  //landmarktransform->PrintSelf(std::cout, vtkIndent());

  ssc::Transform3D tar_M_src(landmarktransform->GetMatrix());

  if (QString::number(tar_M_src[0][0])=="nan") // harry but quick way to check badness of transform...
  {
    ssc::messageManager()->sendError("landmark transform failed");
    return ssc::Transform3D();
  }

  *ok = true;
  return tar_M_src;
}

void RegistrationManager::doPatientRegistration()
{
  if(!mMasterImage)
    return;

  ssc::LandmarkMap masterLandmarks = mMasterImage->getLandmarks();
  ssc::LandmarkMap toolLandmarks = ssc::toolManager()->getLandmarks();

  std::vector<std::string> landmarks = this->getUsableLandmarks(masterLandmarks, toolLandmarks);

  vtkPointsPtr p_ref = this->convertTovtkPoints(landmarks, masterLandmarks, mMasterImage->get_rMd());
  vtkPointsPtr p_pr = this->convertTovtkPoints(landmarks, toolLandmarks, ssc::Transform3D());

  bool ok = false;
  ssc::Transform3D rMpr = this->performLandmarkRegistration(p_pr, p_ref, &ok);
  if (!ok)
    return;

  ssc::RegistrationTransform regTrans(rMpr, QDateTime::currentDateTime(), "Patient");
  ssc::toolManager()->get_rMpr_History()->updateRegistration(mLastRegistrationTime, regTrans);
  mLastRegistrationTime = regTrans.mTimestamp;

  mPatientRegistrationOffset = ssc::Transform3D();

  emit patientRegistrationPerformed();
  ssc::messageManager()->sendInfo("Patient registration has been performed.");
}

void RegistrationManager::doImageRegistration(ssc::ImagePtr image)
{
  //check that the masterimage is set
  if(!mMasterImage)
  {
    return;
  }

  // ignore self-registration, this gives no effect bestcase, buggy behaviour worstcase (has been observed)
  if (image==mMasterImage)
  {
	  return;
  }

  ssc::LandmarkMap masterLandmarks = mMasterImage->getLandmarks();
  ssc::LandmarkMap imageLandmarks = image->getLandmarks();

  std::vector<std::string> landmarks = getUsableLandmarks(masterLandmarks, imageLandmarks);
  vtkPointsPtr p_ref = convertTovtkPoints(landmarks, masterLandmarks, mMasterImage->get_rMd());
  vtkPointsPtr p_data = convertTovtkPoints(landmarks, imageLandmarks, ssc::Transform3D());

  if (landmarks.empty())
    return;

  bool ok = false;
  ssc::Transform3D rMd = this->performLandmarkRegistration(p_data, p_ref, &ok);
  if (!ok)
    return;

  ssc::Transform3D delta = rMd * image->get_rMd().inv();

  ssc::RegistrationTransform regTrans(delta, QDateTime::currentDateTime(), "Image to Image");
  this->updateRegistration(mLastRegistrationTime, regTrans, image);

  mLastRegistrationTime = regTrans.mTimestamp;

  emit imageRegistrationPerformed();
  ssc::messageManager()->sendInfo("Image registration has been performed.");
}

void RegistrationManager::addXml(QDomNode& parentNode)
{
  QDomDocument doc = parentNode.ownerDocument();
  QDomElement base = doc.createElement("registrationManager");
  parentNode.appendChild(base);

  QDomElement masterImageNode = doc.createElement("masterImageUid");
  if(mMasterImage)
  {
    masterImageNode.appendChild(doc.createTextNode(mMasterImage->getUid().c_str()));
  }
  base.appendChild(masterImageNode);
}

void RegistrationManager::parseXml(QDomNode& dataNode)
{
  //ssc::messageManager()->sendInfo("Inside: void RegistrationManager::parseXml(QDomNode& dataNode)");
  QDomNode child = dataNode.firstChild();
  while(!child.isNull())
  {
    if(child.toElement().tagName() == "masterImageUid")
    {
      const QString masterImageString = child.toElement().text();
      //std::cout << "RM: Found a masterImage with uid: " << masterImageString.toStdString().c_str() << std::endl;
      if(!masterImageString.isEmpty())
      {
        ssc::ImagePtr image = ssc::dataManager()->getImage(masterImageString.toStdString());
        //std::cout << "RM: Got an image with uid: " << image->getUid().c_str() << std::endl;
        this->setMasterImage(image);
      }
    }
    child = child.nextSibling();
  }
}

void RegistrationManager::clear()
{
  mLastRegistrationTime = QDateTime();
  this->setMasterImage(ssc::ImagePtr());
}


}//namespace cx
