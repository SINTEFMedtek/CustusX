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
void RegistrationManager::setGlobalPointSetNameList(RegistrationManager::NameListType nameList)
{
  mGlobalPointSetNameList = nameList;
}
RegistrationManager::NameListType RegistrationManager::getGlobalPointSetNameList()
{
  return mGlobalPointSetNameList;
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
