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

#include "cxLegacySingletons.h"

#include "sscDataManager.h"
#include "sscToolManager.h"
#include "cxSpaceProviderImpl.h"

namespace cx
{

//DataManager* dataManager()
//{
//	return DataManager::getInstance();
//}

//ToolManager* toolManager()
//{
//	return ToolManager::getInstance();
//}

SpaceProviderPtr spaceProvider()
{
	return SpaceProviderPtr(new SpaceProviderImpl(toolManager(), dataManager()));
}

} // namespace cx


