/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/


#include "cxDataManager.h"

#include "cxTransferFunctions3DPresets.h"
#include "cxMesh.h"
#include "cxTrackedStream.h"

namespace cx
{

DataManager::DataManager()
{
}

DataManager::~DataManager()
{
}

PresetTransferFunctions3DPtr DataManager::getPresetTransferFunctions3D() const
{
	return PresetTransferFunctions3DPtr(new TransferFunctions3DPresets(XmlOptionFile(), XmlOptionFile()));
}

ImagePtr DataManager::getImage(const QString &uid) const
{
	return boost::dynamic_pointer_cast<Image>(this->getData(uid));
}

MeshPtr DataManager::getMesh(const QString &uid) const
{
	return boost::dynamic_pointer_cast<Mesh>(this->getData(uid));
}

TrackedStreamPtr DataManager::getTrackedStream(const QString &uid) const
{
	return boost::dynamic_pointer_cast<TrackedStream>(this->getData(uid));
}

} // namespace cx
