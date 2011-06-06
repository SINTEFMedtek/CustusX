/*
 * cxLogicManager.cpp
 *
 *  Created on: Jun 1, 2011
 *      Author: christiana
 */

#include <cxLogicManager.h>

#include "cxServiceController.h"

namespace cx
{

// --------------------------------------------------------
LogicManager* LogicManager::mInstance = NULL; ///< static member
// --------------------------------------------------------

void LogicManager::initialize()
{
	LogicManager::getInstance();
}

void LogicManager::shutdown()
{
  delete mInstance;
  mInstance = NULL;
}

LogicManager* LogicManager::getInstance()
{
	if (!mInstance)
	{
		mInstance = new LogicManager;
	}
	return mInstance;
}


LogicManager::LogicManager()
{
	mServiceController.reset(new ServiceController);
}

LogicManager::~LogicManager()
{

}


}
