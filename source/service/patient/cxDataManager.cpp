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

#include "sscXmlOptionItem.h"
#include "cxDataLocations.h"
#include "sscTransferFunctions3DPresets.h"

namespace cx
{

void cxDataManager::initialize()
{
	setInstance(new cxDataManager());
}

cxDataManager* cxDataManager::getInstance()
{
	return dynamic_cast<cxDataManager*>(DataManager::getInstance());
}

cxDataManager::cxDataManager() :
				mDebugMode(false)
{
}

cxDataManager::~cxDataManager()
{
}

PresetTransferFunctions3DPtr cxDataManager::getPresetTransferFunctions3D() const
{
	///< create from filename, create trivial document of type name and root node if no file exists.
	XmlOptionFile preset = XmlOptionFile(
					DataLocations::getRootConfigPath() + "/transferFunctions/presets.xml", "transferFunctions");
	XmlOptionFile custom = XmlOptionFile(DataLocations::getXmlSettingsFile(), "CustusX").descend(
					"presetTransferFunctions");

	if (!mPresetTransferFunctions3D)
		mPresetTransferFunctions3D.reset(new TransferFunctions3DPresets(preset, custom));

	return mPresetTransferFunctions3D;
}

bool cxDataManager::getDebugMode() const
{
	return mDebugMode;
}
void cxDataManager::setDebugMode(bool on)
{
	if (mDebugMode == on)
		return;
	std::cout << "Setting DEBUG MODE = " << on << std::endl;
	mDebugMode = on;
	emit debugModeChanged(mDebugMode);
}

} //namespace cx
