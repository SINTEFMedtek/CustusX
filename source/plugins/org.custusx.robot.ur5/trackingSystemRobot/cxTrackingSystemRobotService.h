#ifndef CXTRACKINGSYSTEMROBOTSERVICE_H
#define CXTRACKINGSYSTEMROBOTSERVICE_H

#include "org_custusx_robot_ur5_Export.h"

#include "cxTrackingService.h"
#include "cxTrackingSystemService.h"
#include "cxUr5Robot.h"

namespace cx
{
typedef boost::shared_ptr<class RobotTool> RobotToolPtr;

class org_custusx_robot_ur5_EXPORT TrackingSystemRobotService : public TrackingSystemService
{
Q_OBJECT

public:
    TrackingSystemRobotService(TrackingServicePtr trackingService, Ur5RobotPtr robot);
    virtual ~TrackingSystemRobotService();

    virtual QString getUid() const { return "org.custusx.robot.ur5"; }
    virtual std::vector<ToolPtr> getTools();

    virtual Tool::State getState() const;
    virtual void setState(const Tool::State val);

    virtual void setLoggingFolder(QString loggingFolder); ///<\param loggingFolder path to the folder where logs should be saved
    virtual TrackerConfigurationPtr getConfiguration();

    TrackingSystemServicePtr getBase() { return mBase; }

    ToolPtr getReference();

private slots:
    void onStateChanged();

private:
    std::vector<ToolPtr> mTools;
    TrackingSystemServicePtr mBase;
    TrackingServicePtr mTrackingService;
    Ur5RobotPtr mUr5Robot;

};

} /* namespace cx */

#endif // TRACKINGSYSTEMROBOTSERVICE_H
