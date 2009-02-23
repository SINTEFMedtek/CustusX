#include "cxRegistrationManager.h"
#include "cxToolmanager.h"
#include "vtkPoints.h"
#include "vtkDoubleArray.h"
#include "vtkLandmarkTransform.h"

/**
 * cxRegistrationManager.cpp
 *
 * \brief
 *
 * \date Feb 4, 2009
 * \author: Janne Beate Bakeng, SINTEF
 */

namespace cx
{
RegistrationManager* RegistrationManager::mCxInstance = NULL;
RegistrationManager* RegistrationManager::getInstance()
{
  if (mCxInstance == NULL)
  {
    mCxInstance = new RegistrationManager();
  }
  return mCxInstance;
}
RegistrationManager::RegistrationManager()
{
  //TODO
}
RegistrationManager::~RegistrationManager()
{}
void RegistrationManager::setMasterImage(ssc::ImagePtr image)
{
  mMasterImage = image;
}
ssc::ImagePtr RegistrationManager::getMasterImage()
{
  return mMasterImage;
}
bool RegistrationManager::isMasterImageSet()
{
  return mMasterImage.get();
}
void RegistrationManager::setGlobalPointSet(vtkDoubleArrayPtr pointset)
{
  mGlobalPointSet = pointset;
}
vtkDoubleArrayPtr RegistrationManager::getGlobalPointSet()
{
  return mGlobalPointSet;
}
void RegistrationManager::setGlobalPointSetNameList(RegistrationManager::NameListType nameList)
{
  mGlobalPointSetNameList = nameList;
}
RegistrationManager::NameListType RegistrationManager::getGlobalPointSetNameList()
{
  return mGlobalPointSetNameList;
}
void RegistrationManager::doPatientRegistration()
{
  // Bør sjekke om masterImage er satt ?
  // Beregne transform kun basert på aktive punkter
  vtkDoubleArrayPtr toolPoints = ToolManager::getInstance()->getToolSamples();
  vtkDoubleArrayPtr imagePoints = this->getMasterImage()->getLandmarks();

  vtkPointsPtr sourcePoints = vtkPointsPtr::New();
  vtkPointsPtr targetPoints = vtkPointsPtr::New();
  vtkLandmarkTransformPtr landmarktransform = vtkLandmarkTransformPtr::New();

	int numberOfPoints = toolPoints->GetNumberOfTuples();
  for (int iPairNum = 0; iPairNum < numberOfPoints; iPairNum++)
  {
    double* sourcePoint = toolPoints->GetTuple(iPairNum);
    double* targetPoint = imagePoints->GetTuple(iPairNum);
    sourcePoints->InsertNextPoint(sourcePoint[0], sourcePoint[1], sourcePoint[2]);
    targetPoints->InsertNextPoint(targetPoint[0], targetPoint[1], targetPoint[2]);
  }
  
  landmarktransform->SetSourceLandmarks(sourcePoints);
  landmarktransform->SetTargetLandmarks(targetPoints);
  landmarktransform->SetModeToSimilarity();
  sourcePoints->Modified();
  targetPoints->Modified();
  landmarktransform->Update();
  

}
void RegistrationManager::doImageRegistration(ssc::ImagePtr image)
{


}

void RegistrationManager::setGlobalPointsNameSlot(int index, std::string name)
{
  if(name.empty())
    return;

  NameListType::iterator it = mGlobalPointSetNameList.find(index);
  if(it != mGlobalPointSetNameList.end())
  {
    it->second.first = name;
  }
  else
  {
    mGlobalPointSetNameList.insert(std::pair<int,StringBoolPair>(index, StringBoolPair(name,true)));
  }
}
void RegistrationManager::setGlobalPointsActiveSlot(int index, bool active)
{
  std::string name = " ";
  NameListType::iterator it = mGlobalPointSetNameList.find(index);
  if(it != mGlobalPointSetNameList.end())
  {
    it->second.second = active;
  }
  else
  {
    mGlobalPointSetNameList.insert(std::pair<int,StringBoolPair>(index, StringBoolPair(name,active)));
  }
}
}//namespace cx
