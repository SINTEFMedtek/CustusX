#include "cxRegistrationManager.h"

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
void RegistrationManager::setGlobalPointSetNameList(std::map<std::string, bool> nameList)
{
  mGlobalPointSetNameList = nameList;
}
std::map<std::string, bool> RegistrationManager::getGlobalPointSetNameList()
{
  return mGlobalPointSetNameList;
}
void RegistrationManager::setGlobalPointsNameSlot(int index, std::string)
{
  //TODO
}
void RegistrationManager::setGlobalPointsActiveSlot(int index, bool active)
{
  //TODO
}
}//namespace cx
