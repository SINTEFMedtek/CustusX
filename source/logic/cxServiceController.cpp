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
#include "sscMessageManager.h"

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

	this->connectVideoToProbe(tool);

  ssc::toolManager()->setDominantTool(tool->getUid());
}

/**insert the rt source into the (first) probe tool
 * in the tool manager.
 *
 * Apply time calibration to the source.
 *
 */
void ServiceController::connectVideoToProbe(ssc::ToolPtr probe)
{
	ssc::VideoSourcePtr source = videoService()->getVideoConnection()->getVideoSource();
  if (!source)
 {
    ssc::messageManager()->sendError("no rt source.");
    return;
 }

//    return;
  // find probe in tool manager
  // set source in cxTool
  // insert timecalibration using config
  if (!source->isConnected())
    return;

//  if (mProbe)
//    return;

//  ssc::ToolPtr probe = this->findSuitableProbe();
  if (!probe)
    return;

  if (!probe->getProbe()->getRTSource())
  	return;

//  mProbe = probe;

  if (probe)
  {
    ProbePtr probeInterface = boost::shared_dynamic_cast<Probe>(probe->getProbe());
    if (!probeInterface)
    {
      ssc::messageManager()->sendError("Probe not a cx instance.");
      return;
    }
    probeInterface->setRTSource(source);
//    ssc::toolManager()->setDominantTool(mProbe->getUid());
//    std::cout << "VideoConnection::connectSourceToTool() " << probe->getUid() << " " << probeInterface->getVideoSource()->getName() << " completed" << std::endl;
  }
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
