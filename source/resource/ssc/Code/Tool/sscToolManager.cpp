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

#include "sscToolManager.h"

namespace ssc
{

ToolManager* toolManager()
{
	return ToolManager::getInstance();
}

ToolManager* ToolManager::mInstance = NULL;

/** not sure if this is needed? we have getInstance in subclasses...*/
void ToolManager::setInstance(ToolManager* instance)
{
	mInstance = instance;
}
;

ToolManager* ToolManager::getInstance()
{
	return mInstance;
}

void ToolManager::shutdown()
{
	delete mInstance;
	mInstance = NULL;
}

}
