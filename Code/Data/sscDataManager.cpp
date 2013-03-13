// This file is part of SSC,
// a C++ Library supporting Image Guided Therapy Applications.
//
// Copyright (C) 2008- SINTEF Medical Technology
// Copyright (C) 2008- Sonowand AS
//
// SSC is owned by SINTEF Medical Technology and Sonowand AS,
// hereafter named the owners. Each particular piece of code
// is owned by the part that added it to the library.
// SSC source code and binaries can only be used by the owners
// and those with explicit permission from the owners.
// SSC shall not be distributed to anyone else.
//
// SSC is distributed WITHOUT ANY WARRANTY; without even
// the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE.
//
// See sscLicense.txt for more information.

#include "sscDataManager.h"

#include "sscDataManagerImpl.h"
#include "sscTransferFunctions3DPresets.h"

namespace ssc
{

// --------------------------------------------------------
DataManager* DataManager::mInstance = NULL; ///< static member
// --------------------------------------------------------

void DataManager::shutdown()
{
	delete mInstance;
	mInstance = NULL;
}

DataManager* DataManager::getInstance()
{
	if (!mInstance)
	{
		DataManagerImpl::initialize();
	}
	return mInstance;
}

void DataManager::setInstance(DataManager* instance)
{
	if (mInstance)
	{
		delete mInstance;
	}
	mInstance = instance;
}

DataManager::DataManager()
{
}

DataManager::~DataManager()
{
}

PresetTransferFunctions3DPtr DataManager::getPresetTransferFunctions3D() const
{
	return ssc::PresetTransferFunctions3DPtr(new ssc::TransferFunctions3DPresets(ssc::XmlOptionFile(), ssc::XmlOptionFile()));
}

ImagePtr DataManager::getActiveImage() const
{
	return ImagePtr();
} ///< used for system state
void DataManager::setActiveImage(ImagePtr activeImage)
{
} ///< used for system state

DataManager* dataManager()
{
	return DataManager::getInstance();
}

} // namespace ssc
