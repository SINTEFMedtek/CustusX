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
DataManager::DataManager()
{}
DataManager::~DataManager()
{}
  
void DataManager::deleteImageSlot(ssc::ImagePtr image)
{
  mImages.erase(image->getUid());
  emit currentImageDeleted(image);
}

}//namespace cx
