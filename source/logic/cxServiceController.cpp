/*
 * cxServiceController.cpp
 *
 *  Created on: Jun 1, 2011
 *      Author: christiana
 */

#include <cxServiceController.h>

#include "cxDataManager.h"
#include "cxToolManager.h"
#include "cxVideoService.h"

namespace cx
{

ServiceController::ServiceController()
{
	// test
	ssc::toolManager();
	ssc::dataManager();
	videoService();

}

ServiceController::~ServiceController()
{
	// TODO Auto-generated destructor stub
}

}
