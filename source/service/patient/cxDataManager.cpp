#include <QDomDocument>
#include "cxDataManager.h"

#include "cxPointMetric.h"
#include "cxDistanceMetric.h"
#include "cxPlaneMetric.h"
#include "cxAngleMetric.h"

namespace cx
{

void DataManager::initialize()
{
  setInstance(new DataManager());

  // extra cx data types
  getInstance()->mDataReaders.insert(ssc::DataReaderPtr(new PointMetricReader()));
  getInstance()->mDataReaders.insert(ssc::DataReaderPtr(new DistanceMetricReader()));
  getInstance()->mDataReaders.insert(ssc::DataReaderPtr(new PlaneMetricReader()));
  getInstance()->mDataReaders.insert(ssc::DataReaderPtr(new AngleMetricReader()));
}

DataManager* DataManager::getInstance()
{
  return dynamic_cast<DataManager*>(ssc::DataManager::getInstance());
//  if (mCxInstance == NULL)
//  {
//    mCxInstance = new DataManager();
//    ssc::DataManager::setInstance(mCxInstance);
//  }
//  return mCxInstance;
}
DataManager::DataManager() : mDebugMode(false)
{}
DataManager::~DataManager()
{
  //std::cout << "DataManager::~DataManager()" << std::endl;
}
  
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
  if (!this->getImage(image->getUid()))
    return;
  mData.erase(image->getUid());
  emit currentImageDeleted(image);
}
//DataManager* dataManager()
//{
//  return DataManager::getInstance();
//}
}//namespace cx
