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
#include "cxMessagemanager.h"

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
RegistrationManager::RegistrationManager() :
  mToolManager(ToolManager::getInstance())
{}
RegistrationManager::~RegistrationManager()
{}
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
void RegistrationManager::setGlobalPointSet(vtkDoubleArrayPtr pointset)
{
  mGlobalPointSet = pointset;
  messageManager()->sendInfo("Global point set is set.");
}
vtkDoubleArrayPtr RegistrationManager::getGlobalPointSet()
{
  return mGlobalPointSet;
}
void RegistrationManager::setGlobalPointSetNameList(RegistrationManager::NameListType nameList)
{
  mGlobalPointSetNameList = nameList;
  messageManager()->sendInfo("Global point set name list is set.");
}
RegistrationManager::NameListType RegistrationManager::getGlobalPointSetNameList()
{
  return mGlobalPointSetNameList;
}
void RegistrationManager::setManualPatientRegistration(ssc::Transform3DPtr patientRegistration)
{
  mManualPatientRegistration = patientRegistration;
  //mToolManager->set_rMpr(patientRegistration);

  ssc::RegistrationTransform regTrans(*patientRegistration, QDateTime::currentDateTime(), "Manual Patient");
  mToolManager->get_rMpr_History()->addRegistration(regTrans);

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
    currentTransform = *mToolManager->get_rMpr();
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
  mToolManager->get_rMpr_History()->addRegistration(regTrans);


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
void RegistrationManager::doPatientRegistration()
{
  if(!mMasterImage)
  {
    messageManager()->sendWarning("Cannot do a patient registration without having a master image. Mark some landmarks in an image and try again.");
    return;
  }

  vtkDoubleArrayPtr toolPoints = ToolManager::getInstance()->getToolSamples();
  vtkDoubleArrayPtr imagePoints = this->getMasterImage()->getLandmarks();

  vtkPointsPtr sourcePoints = vtkPointsPtr::New();
  vtkPointsPtr targetPoints = vtkPointsPtr::New();
  vtkLandmarkTransformPtr landmarktransform = vtkLandmarkTransformPtr::New();

	int numberOfToolPoints = toolPoints->GetNumberOfTuples();
	int numberOfImagePoints = imagePoints->GetNumberOfTuples();

  for (int i=0; i < numberOfToolPoints; i++)
  {
    for(int j=0; j < numberOfImagePoints; j++)
    {
      double* sourcePoint = toolPoints->GetTuple(i);
      double* targetPoint = imagePoints->GetTuple(j);
      if(sourcePoint[3] == targetPoint[3])
      {
        NameListType::iterator it = mGlobalPointSetNameList.find(sourcePoint[3]);
        if(it->second.second)
        {
          // Insert pointset if state is active
          sourcePoints->InsertNextPoint(sourcePoint[0], sourcePoint[1], sourcePoint[2]);
          targetPoints->InsertNextPoint(targetPoint[0], targetPoint[1], targetPoint[2]);
        }
      }
    }
  }

  landmarktransform->SetSourceLandmarks(sourcePoints);
  landmarktransform->SetTargetLandmarks(targetPoints);
  landmarktransform->SetModeToSimilarity();
  sourcePoints->Modified();
  targetPoints->Modified();
  landmarktransform->Update();

  //update rMpr transform in ToolManager
  //vtkMatrix4x4* matrix = landmarktransform->GetMatrix();
  ssc::Transform3D rMpr(landmarktransform->GetMatrix());

//  ssc::Transform3DPtr rMprPtr(new ssc::Transform3D(matrix));
//  mToolManager->set_rMpr(rMprPtr);

  ssc::RegistrationTransform regTrans(rMpr, QDateTime::currentDateTime(), "Patient");
  //image->get_rMd_History()->addRegistration(regTrans);
  mToolManager->get_rMpr_History()->addRegistration(regTrans);

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
  
  //calculate the transform from image to dataRef
  vtkDoubleArrayPtr imagePoints = image->getLandmarks();
  vtkDoubleArrayPtr masterImagePoints = mMasterImage->getLandmarks();

  vtkPointsPtr sourcePoints = vtkPointsPtr::New();
  vtkPointsPtr targetPoints = vtkPointsPtr::New();
  vtkLandmarkTransformPtr landmarktransform = vtkLandmarkTransformPtr::New();

  int numberOfImagePoints = imagePoints->GetNumberOfTuples();
  int numberOfMasterImagePoints = masterImagePoints->GetNumberOfTuples();
  for (int i=0; i < numberOfImagePoints; i++)
  {
    for(int j=0; j < numberOfMasterImagePoints; j++)
    {
      double* sourcePoint = imagePoints->GetTuple(i);
      double* targetPoint = masterImagePoints->GetTuple(j);
      if(sourcePoint[3] == targetPoint[3])
      {
        NameListType::iterator it = mGlobalPointSetNameList.find(sourcePoint[3]);
        if(it->second.second)
        {
          sourcePoints->InsertNextPoint(sourcePoint[0], sourcePoint[1], sourcePoint[2]);
          targetPoints->InsertNextPoint(targetPoint[0], targetPoint[1], targetPoint[2]);
        }
      }
    }
  }

  landmarktransform->SetSourceLandmarks(sourcePoints);
  landmarktransform->SetTargetLandmarks(targetPoints);
  landmarktransform->SetModeToSimilarity();
  sourcePoints->Modified();
  targetPoints->Modified();
  landmarktransform->Update();

  //set the transform on the image
  vtkMatrix4x4* matrix = landmarktransform->GetMatrix();
  ssc::Transform3D transform(matrix);
  //image->set_rMd(transform);
  ssc::RegistrationTransform regTrans(transform, QDateTime::currentDateTime(), "Image to Image");
  image->get_rMd_History()->addRegistration(regTrans);
  //why did we use the inverse of the transform?
  //image->set_rMd(transform.inv());//set_rMd() must have an inverted transform wrt the removed setTransform()

  emit imageRegistrationPerformed();
  messageManager()->sendInfo("Image registration has been performed.");
}

void RegistrationManager::setGlobalPointsNameSlot(int index, std::string name)
{
  if(name.empty())
    return;

  NameListType::iterator it = mGlobalPointSetNameList.find(index);
  if(it != mGlobalPointSetNameList.end())
  {
    it->second.first = name;
    messageManager()->sendInfo("Updated name for existing global point to: "+name);
  }
  else
  {
    mGlobalPointSetNameList.insert(std::pair<int,StringBoolPair>(index, StringBoolPair(name,true)));
    messageManager()->sendInfo("Created new global point name with name: "+name);
  }
}
void RegistrationManager::setGlobalPointsActiveSlot(int index, bool active)
{
  std::string name = " ";
  NameListType::iterator it = mGlobalPointSetNameList.find(index);
  if(it != mGlobalPointSetNameList.end())
  {
    it->second.second = active;
    messageManager()->sendInfo("Updated status to for existing point.");
  }
  else
  {
    mGlobalPointSetNameList.insert(std::pair<int,StringBoolPair>(index, StringBoolPair(name,active)));
    messageManager()->sendInfo("Added new point with active status.");
  }
}
}//namespace cx
