#include "cxRegistrationManager.h"

#include "vtkMath.h"
#include "vtkPoints.h"
#include "vtkDoubleArray.h"
#include "vtkLandmarkTransform.h"
#include "vtkMatrix4x4.h"
#include "sscTransform3D.h"
#include "cxToolmanager.h"

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
RegistrationManager::RegistrationManager() :
  mToolManager(ToolManager::getInstance())
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
void RegistrationManager::setActivePointsMap(std::map<int, bool> vector)
{
  mActivePointsMap = vector;
}
std::map<int, bool> RegistrationManager::getActivePointsMap()
{
  return mActivePointsMap;
}
void RegistrationManager::doPatientRegistration()
{
  // TODO:
  // Bør sjekke om masterImage er satt ?
  // Beregne transform kun basert på aktive punkter
  vtkDoubleArrayPtr toolPoints = ToolManager::getInstance()->getToolSamples();
  std::cout << "-----TOOLPOINTS----- " << std::endl;
  toolPoints->Print(std::cout);
  for(int i=0;i<toolPoints->GetNumberOfTuples();i++)
  {
    double* toolPoint = toolPoints->GetTuple(i);
    std::cout << "Toolpoint: "<< i <<": (" << toolPoint[0] << "," << toolPoint[1] << "," << toolPoint[2] << ")" << std::endl;
  }
  std::cout << "-----TOOLPOINTS----- " << std::endl;
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
      std::cout << "sourcePoint[3] == targetPoint[3] : (" << sourcePoint[3] << "," << targetPoint[3] << ")" << std::endl;
      if(sourcePoint[3] == targetPoint[3])
      {
        std::map<int, bool>::iterator it = mActivePointsMap.find(sourcePoint[3]);
        if(it->second)
        {
          // Insert pointset if state is active
          std::cout << "Index: " << sourcePoint[3] << std::endl;;
          sourcePoints->InsertNextPoint(sourcePoint[0], sourcePoint[1], sourcePoint[2]);
          targetPoints->InsertNextPoint(targetPoint[0], targetPoint[1], targetPoint[2]);
        }
      }
    }
  }

  std::cout << "-----SOURCE_POINTS----- " << std::endl;
  sourcePoints->Print(std::cout);
  std::cout << "-----SOURCE_POINTS----- " << std::endl;
  std::cout << "-----TARGET_POINTS----- " << std::endl;
  targetPoints->Print(std::cout);
  std::cout << "-----TARGET_POINTS----- " << std::endl;

  landmarktransform->SetSourceLandmarks(sourcePoints);
  landmarktransform->SetTargetLandmarks(targetPoints);
  landmarktransform->SetModeToSimilarity();
  sourcePoints->Modified();
  targetPoints->Modified();
  landmarktransform->Update();

  std::cout << "-----LANDMARKTRANSFORM----- " << std::endl;
  landmarktransform->Print(std::cout);
  std::cout << "-----LANDMARKTRANSFORM----- " << std::endl;

  //update rMpr transform in ToolManager
  vtkMatrix4x4* matrix = landmarktransform->GetMatrix();

  std::cout << "-----BEFORE_MATRIX----- " << std::endl;
  matrix->Print(std::cout);
  std::cout << "-----BEFORE_MATRIX----- " << std::endl;

//  //Trying to normalize...
//  double row1[3], row2[3], row3[3];
//
//  //row1
//  row1[0] = matrix->GetElement(0,0);
//  row1[1] = matrix->GetElement(1,0);
//  row1[2] = matrix->GetElement(2,0);
//  //row2
//  row2[0] = matrix->GetElement(0,1);
//  row2[1] = matrix->GetElement(1,1);
//  row2[2] = matrix->GetElement(2,1);
//  //row3
//  row3[0] = matrix->GetElement(0,2);
//  row3[1] = matrix->GetElement(1,2);
//  row3[2] = matrix->GetElement(2,2);
//
//  vtkMath::Normalize(row1);
//  vtkMath::Normalize(row2);
//  vtkMath::Normalize(row3);
//
//  //row1
//  matrix->SetElement(0,0,row1[0]);
//  matrix->SetElement(1,0,row1[1]);
//  matrix->SetElement(2,0,row1[2]);
//  //row2
//  matrix->SetElement(0,1,row2[0]);
//  matrix->SetElement(1,1,row2[1]);
//  matrix->SetElement(2,1,row2[2]);
//  //row3
//  matrix->SetElement(0,2,row3[0]);
//  matrix->SetElement(1,2,row3[1]);
//  matrix->SetElement(2,2,row3[2]);
//
//  matrix->Modified();
//
//  std::cout << "-----AFTER_MATRIX----- " << std::endl;
//  matrix->Print(std::cout);
//  std::cout << "-----AFTER_MATRIX----- " << std::endl;

  ssc::Transform3DPtr rMprPtr(new ssc::Transform3D(matrix));
  mToolManager->set_rMpr(rMprPtr);
}
void RegistrationManager::doImageRegistration(ssc::ImagePtr image)
{
  //TODO
  //use only active points
  //check that the active points exists in the image

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
        sourcePoints->InsertNextPoint(sourcePoint[0], sourcePoint[1], sourcePoint[2]);
        targetPoints->InsertNextPoint(targetPoint[0], targetPoint[1], targetPoint[2]);
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
  image->setTransform(transform);
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
