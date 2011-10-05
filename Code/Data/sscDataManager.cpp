#include "sscDataManager.h"

#include <vtkImageData.h>
#include <vtkMetaImageReader.h>

#include <vtkPolyData.h>
#include <vtkPolyDataReader.h>
#include <vtkSTLReader.h>

#include "sscDataManagerImpl.h"
#include "sscImage.h"
#include "sscMesh.h"
#include "sscPresetTransferFunctions3D.h"

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
	return ssc::PresetTransferFunctions3DPtr(new ssc::PresetTransferFunctions3D(ssc::XmlOptionFile(), ssc::XmlOptionFile()));
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
