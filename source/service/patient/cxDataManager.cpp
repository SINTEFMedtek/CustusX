// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.

#include <QDomDocument>
#include "cxDataManager.h"

#include "sscPointMetric.h"
#include "sscDistanceMetric.h"
#include "sscPlaneMetric.h"
#include "sscAngleMetric.h"
#include "sscXmlOptionItem.h"
#include "cxDataLocations.h"
#include "sscPresetTransferFunctions3D.h"

namespace cx
{

void DataManager::initialize()
{
	setInstance(new DataManager());

	// extra cx data types
	getInstance()->mDataReaders.insert(ssc::DataReaderPtr(new ssc::PointMetricReader()));
	getInstance()->mDataReaders.insert(ssc::DataReaderPtr(new ssc::DistanceMetricReader()));
	getInstance()->mDataReaders.insert(ssc::DataReaderPtr(new ssc::PlaneMetricReader()));
	getInstance()->mDataReaders.insert(ssc::DataReaderPtr(new ssc::AngleMetricReader()));
}

DataManager* DataManager::getInstance()
{
	return dynamic_cast<DataManager*>(ssc::DataManager::getInstance());
}

DataManager::DataManager() :
				mDebugMode(false)
{
}

DataManager::~DataManager()
{
}

ssc::PresetTransferFunctions3DPtr DataManager::getPresetTransferFunctions3D() const
{
	///< create from filename, create trivial document of type name and root node if no file exists.
	ssc::XmlOptionFile preset = ssc::XmlOptionFile(
					DataLocations::getRootConfigPath() + "/transferFunctions/presets.xml", "transferFunctions");
	ssc::XmlOptionFile custom = ssc::XmlOptionFile(DataLocations::getXmlSettingsFile(), "CustusX").descend(
					"presetTransferFunctions");

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
	if (mDebugMode == on)
		return;
	std::cout << "Setting DEBUG MODE = " << on << std::endl;
	mDebugMode = on;
	emit debugModeChanged(mDebugMode);
}

} //namespace cx
