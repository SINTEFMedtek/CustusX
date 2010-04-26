#include <QDomDocument>
#include "cxDataManager.h"

namespace cx
{
DataManager* DataManager::mCxInstance = NULL;

DataManager* DataManager::getInstance()
{
  if (mCxInstance == NULL)
  {
    mCxInstance = new DataManager();
    ssc::DataManager::setInstance(mCxInstance);
  }
  return mCxInstance;
}
DataManager::DataManager() : mDebugMode(false)
{}
DataManager::~DataManager()
{}
  
bool DataManager::getDebugMode() const
{
  return mDebugMode;
}
void DataManager::setDebugMode(bool on)
{
  if (mDebugMode==on)
    return;
  std::cout << "Setting DEBUG MODE = " << on << std::endl;
  mDebugMode = on;
  emit debugModeChanged(mDebugMode);
}

void DataManager::deleteImageSlot(ssc::ImagePtr image)
{
  mImages.erase(image->getUid());
  emit currentImageDeleted(image);
}
DataManager* dataManager()
{
  return DataManager::getInstance();
}
}//namespace cx
