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
void DataManager::addXml(QDomNode& parentNode)
{
  ssc::DataManagerImpl::addXml(parentNode);
}
void DataManager::parseXml(QDomNode& node)
{
  ssc::DataManagerImpl::parseXml(node);
}

}//namespace cx
