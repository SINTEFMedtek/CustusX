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

void DataManager::save(QString filename)
{
  QDomNode datanode = this->GetXml();
}

void DataManager::load(QString filename)
{
  QDomNode datanode;
  this->ParseXml(&datanode);
}

QDomNode DataManager::GetXml()
{
  QDomNode datanode;
  return datanode;
}

void DataManager::ParseXml(QDomNode* datanode)
{}

}//namespace cx
