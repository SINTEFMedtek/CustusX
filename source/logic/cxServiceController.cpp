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
	// load the ever-present video stream into the patient service
  ssc::dataManager()->loadStream(videoService()->getVideoConnection()->getVideoSource());

  // connecting the video source and the tracking us probe.
	connect(ssc::toolManager(), SIGNAL(configured()), this, SLOT(updateVideoConnections()));
	connect(ssc::toolManager(), SIGNAL(initialized()), this, SLOT(updateVideoConnections()));
  connect(videoService()->getVideoConnection().get(), SIGNAL(connected(bool)), this, SLOT(updateVideoConnections()));
}

ServiceController::~ServiceController()
{
}

/**Connect a probe from Tracking Service to a video source in Video Service.
 *
 */
void ServiceController::updateVideoConnections()
{
	ssc::ToolPtr tool = this->findSuitableProbe();

	videoService()->getVideoConnection()->connectVideoToProbe(tool);
  ssc::toolManager()->setDominantTool(tool->getUid());
}

/**Find a probe that can be connected to a rt source.
 *
 */
ssc::ToolPtr ServiceController::findSuitableProbe()
{
  ssc::ToolManager::ToolMapPtr tools = ssc::toolManager()->getTools();

  // look for visible probes
  for (ssc::ToolManager::ToolMap::iterator iter=tools->begin(); iter!=tools->end(); ++iter)
  {
    if (iter->second->getProbe() && iter->second->getProbe()->isValid() && iter->second->getVisible())
    {
      return iter->second;
    }
  }

  // pick the first probe, visible or not.
  for (ssc::ToolManager::ToolMap::iterator iter=tools->begin(); iter!=tools->end(); ++iter)
  {
    if (iter->second->getProbe() && iter->second->getProbe()->isValid())
    {
      return iter->second;
    }
  }

  return ssc::ToolPtr();
}


}
