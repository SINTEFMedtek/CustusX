#include "cxRegistrationManager.h"

#include <QtCore>
#include "vtkMath.h"
#include "vtkPoints.h"
#include "vtkDoubleArray.h"
#include "vtkLandmarkTransform.h"
#include "vtkMatrix4x4.h"
#include "sscTransform3D.h"
#include "sscRegistrationTransform.h"
#include "cxToolmanager.h"
#include "cxMessageManager.h"
#include "cxDataManager.h"

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
RegistrationManager::RegistrationManager()
  //mToolManager(ToolManager::getInstance())
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
  mMasterImage = image;
  messageManager()->sendInfo("Master image set to "+image->getUid());
}
ssc::ImagePtr RegistrationManager::getMasterImage()
{
  return mMasterImage;
}
bool RegistrationManager::isMasterImageSet()
{
  return mMasterImage;
}
//void RegistrationManager::setGlobalPointSet(vtkDoubleArrayPtr pointset)
//{
//  mGlobalPointSet = pointset;
//  messageManager()->sendInfo("Global point set is set.");
//}
//vtkDoubleArrayPtr RegistrationManager::getGlobalPointSet()
//{
//  return mGlobalPointSet;
//}
//void RegistrationManager::setGlobalPointSetNameList(RegistrationManager::NameListType nameList)
//{
//  mGlobalPointSetNameList = nameList;
//  messageManager()->sendInfo("Global point set name list is set.");
//}
//RegistrationManager::NameListType RegistrationManager::getGlobalPointSetNameList()
//{
//  return mGlobalPointSetNameList;
//}
void RegistrationManager::setManualPatientRegistration(ssc::Transform3DPtr patientRegistration)
{
  mManualPatientRegistration = patientRegistration;
  //mToolManager->set_rMpr(patientRegistration);

  ssc::RegistrationTransform regTrans(*patientRegistration, QDateTime::currentDateTime(), "Manual Patient");
  toolManager()->get_rMpr_History()->addRegistration(regTrans);

  //if an offset existed, its no longer valid and should be removed
  mPatientRegistrationOffset.reset();

  messageManager()->sendInfo("Manual patient registration is set.");
}
ssc::Transform3DPtr RegistrationManager::getManualPatientRegistration()
{
  return mManualPatientRegistration;
}
void RegistrationManager::resetManualPatientientRegistration()
{
  mManualPatientRegistration.reset();
  this->doPatientRegistration();
}
void RegistrationManager::setManualPatientRegistrationOffsetSlot(
    ssc::Transform3DPtr offset)
{
  ssc::Transform3D currentTransform;
  if (mManualPatientRegistration) //we use this if we have it
  {
    currentTransform = *mManualPatientRegistration;
  }
  else if (mMasterImage)
  {
    this->resetOffset();
    currentTransform = *toolManager()->get_rMpr();
  }
  else //if we dont have a masterimage or a manualtransform we just want to save the offset?
  {
    //mPatientRegistrationOffset = offset; ?
    return;
  }
  mPatientRegistrationOffset = offset;
  ssc::Transform3D newTransform = (*mPatientRegistrationOffset) * currentTransform;
//  mToolManager->set_rMpr(newTransformPtr);
  ssc::RegistrationTransform regTrans(newTransform, QDateTime::currentDateTime(), "Manual Patient Offset");
  toolManager()->get_rMpr_History()->addRegistration(regTrans);


  messageManager()->sendInfo("Offset for the patient registration is set.");
}

ssc::Transform3DPtr RegistrationManager::getManualPatientRegistrationOffset()
{
  return mPatientRegistrationOffset;
}
void RegistrationManager::resetOffset()
{
  mPatientRegistrationOffset.reset();
  this->doPatientRegistration();
}


/**Inspect the landmarks in data a and b, find landmarks defined in both of them and
 * that also is active.
 * Return the uids of these landmarks.
 */
std::vector<std::string> RegistrationManager::getUsableLandmarks(const ssc::LandmarkMap& data_a, const ssc::LandmarkMap& data_b)
{
  std::vector<std::string> retval;
  std::map<std::string, ssc::LandmarkProperty> props = dataManager()->getLandmarkProperties();
  std::map<std::string, ssc::LandmarkProperty>::iterator iter;

  for (iter=props.begin(); iter!=props.end(); ++iter)
  {
    std::string uid = iter->first;
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
    std::cout << "not enough points to register" << std::endl;
    return ssc::Transform3D();
  }

  vtkLandmarkTransformPtr landmarktransform = vtkLandmarkTransformPtr::New();
  landmarktransform->SetSourceLandmarks(source);
  landmarktransform->SetTargetLandmarks(target);
  landmarktransform->SetModeToSimilarity();
  source->Modified();
  target->Modified();
  landmarktransform->Update();

  ssc::Transform3D tar_M_src(landmarktransform->GetMatrix());
  *ok = true;
  return tar_M_src;
}


void RegistrationManager::doPatientRegistration()
{
  if(!mMasterImage)
  {
    messageManager()->sendWarning("Cannot do a patient registration without having a master image. Mark some landmarks in an image and try again.");
    return;
  }

  ssc::LandmarkMap masterLandmarks = mMasterImage->getLandmarks();
  ssc::LandmarkMap toolLandmarks = toolManager()->getLandmarks();

  std::vector<std::string> landmarks = this->getUsableLandmarks(masterLandmarks, toolLandmarks);

  vtkPointsPtr p_ref = this->convertTovtkPoints(landmarks, masterLandmarks, mMasterImage->get_rMd());
  vtkPointsPtr p_pr = this->convertTovtkPoints(landmarks, toolLandmarks, ssc::Transform3D());

  bool ok = false;
  ssc::Transform3D rMpr = this->performLandmarkRegistration(p_pr, p_ref, &ok);
  if (!ok)
    return;

//
//  bool ok = false;
//  ssc::Transform3D rMpr = this->performLandmarkRegistration(toolLandmarks, imageLandmarks, &ok);
//  if (!ok)
//    return;

  ssc::RegistrationTransform regTrans(rMpr, QDateTime::currentDateTime(), "Patient");
  toolManager()->get_rMpr_History()->updateRegistration(mLastRegistrationTime, regTrans);
  mLastRegistrationTime = regTrans.mTimestamp;

  emit patientRegistrationPerformed();
  messageManager()->sendInfo("Patient registration has been performed.");
}

void RegistrationManager::doImageRegistration(ssc::ImagePtr image)
{
  //check that the masterimage is set
  if(!mMasterImage)
  {
    messageManager()->sendError("There isn't set a masterimage in the registrationmanager.");
    return;
  }

  ssc::LandmarkMap masterLandmarks = mMasterImage->getLandmarks();
  ssc::LandmarkMap imageLandmarks = image->getLandmarks();

  std::vector<std::string> landmarks = getUsableLandmarks(masterLandmarks, imageLandmarks);
  vtkPointsPtr p_ref = convertTovtkPoints(landmarks, masterLandmarks, mMasterImage->get_rMd());
  vtkPointsPtr p_data = convertTovtkPoints(landmarks, imageLandmarks, ssc::Transform3D());

  bool ok = false;
  ssc::Transform3D rMd = this->performLandmarkRegistration(p_data, p_ref, &ok);
  if (!ok)
    return;

  ssc::RegistrationTransform regTrans(rMd, QDateTime::currentDateTime(), "Image to Image");
  image->get_rMd_History()->updateRegistration(mLastRegistrationTime, regTrans);
  mLastRegistrationTime = regTrans.mTimestamp;

  //why did we use the inverse of the transform?
  //image->set_rMd(transform.inv());//set_rMd() must have an inverted transform wrt the removed setTransform()

  emit imageRegistrationPerformed();
  messageManager()->sendInfo("Image registration has been performed.");
}

//void RegistrationManager::setGlobalPointsNameSlot(int index, std::string name)
//{
//  if(name.empty())
//    return;
//
//  NameListType::iterator it = mGlobalPointSetNameList.find(index);
//  if(it != mGlobalPointSetNameList.end())
//  {
//    it->second.first = name;
//    messageManager()->sendInfo("Updated name for existing global point to: "+name);
//  }
//  else
//  {
//    mGlobalPointSetNameList.insert(std::pair<int,StringBoolPair>(index, StringBoolPair(name,true)));
//    messageManager()->sendInfo("Created new global point name with name: "+name);
//  }
//}
//void RegistrationManager::setGlobalPointsActiveSlot(int index, bool active)
//{
//  std::string name = " ";
//  NameListType::iterator it = mGlobalPointSetNameList.find(index);
//  if(it != mGlobalPointSetNameList.end())
//  {
//    it->second.second = active;
//    messageManager()->sendInfo("Updated status to for existing point.");
//  }
//  else
//  {
//    mGlobalPointSetNameList.insert(std::pair<int,StringBoolPair>(index, StringBoolPair(name,active)));
//    messageManager()->sendInfo("Added new point with active status.");
//  }
//}
}//namespace cx
