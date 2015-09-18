
#include "cxTrackingSystemRobotService.h"

#include "cxRobotTool.h"

namespace cx
{

TrackingSystemRobotService::TrackingSystemRobotService(TrackingServicePtr trackingService, Ur5RobotPtr robot):
    mBase(trackingService->getTrackingSystems().front()),
    mTrackingService(trackingService),
    mUr5Robot(robot)
{
    connect(mBase.get(),&TrackingSystemService::stateChanged, this, &TrackingSystemRobotService::onStateChanged);
    this->onStateChanged();
}

TrackingSystemRobotService::~TrackingSystemRobotService()
{
}

void TrackingSystemRobotService::onStateChanged()
{
    ToolPtr activeTool = mTrackingService->getActiveTool();
    std::vector<ToolPtr> tools = mBase->getTools();
    mTools.clear();

    for(unsigned i=0; i<tools.size(); ++i)
    {
        if(tools[i] == activeTool)
        {
            RobotToolPtr current(new RobotTool(tools[i],mUr5Robot));
            mTools.push_back(current);
        }
        else
        {
            mTools.push_back(tools[i]);
        }
    }
    emit stateChanged();
}

ToolPtr TrackingSystemRobotService::getReference()
{
    return mBase->getReference();
}


std::vector<ToolPtr> TrackingSystemRobotService::getTools()
{
    return mTools;
}

Tool::State TrackingSystemRobotService::getState() const
{
    return mBase->getState();
}

void TrackingSystemRobotService::setState(const Tool::State val)
{
    mBase->setState(val);
}

void TrackingSystemRobotService::setLoggingFolder(QString loggingFolder)
{
    mBase->setLoggingFolder(loggingFolder);
}

TrackerConfigurationPtr TrackingSystemRobotService::getConfiguration()
{
    return mBase->getConfiguration();
}


} // namespace cx
