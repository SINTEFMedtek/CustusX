#include <QDomDocument>
#include "cxDataManager.h"

#include "cxPointMetric.h"
#include "cxDistanceMetric.h"
#include "cxPlaneMetric.h"
#include "cxAngleMetric.h"
#include "sscXmlOptionItem.h"
#include "cxDataLocations.h"
#include "sscPresetTransferFunctions3D.h"

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
}

DataManager::DataManager() : mDebugMode(false)
{
}

DataManager::~DataManager()
{
}
  
ssc::PresetTransferFunctions3DPtr DataManager::getPresetTransferFunctions3D() const
{
	///< create from filename, create trivial document of type name and root node if no file exists.
	ssc::XmlOptionFile preset = ssc::XmlOptionFile(DataLocations::getRootConfigPath()+"/transferFunctions/presets.xml", "transferFunctions");
	ssc::XmlOptionFile custom = ssc::XmlOptionFile(DataLocations::getXmlSettingsFile(),"CustusX").descend("presetTransferFunctions");

	if (!mPresetTransferFunctions3D)
		mPresetTransferFunctions3D.reset(new ssc::PresetTransferFunctions3D(preset, custom));

	return mPresetTransferFunctions3D;
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

}//namespace cx
