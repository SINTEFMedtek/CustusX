#ifndef CXROBOTTRACKINGSYSTEMSERVICE_H
#define CXROBOTTRACKINGSYSTEMSERVICE_H

#include "org_custusx_robot_ur5_Export.h"

#include "cxTrackingService.h"
#include "cxTrackingSystemService.h"

#include "cxRobotTool.h"
#include "cxUr5Robot.h"

namespace cx
{
typedef boost::shared_ptr<class RobotTrackingSystemService> RobotTrackingSystemServicePtr;

class org_custusx_robot_ur5_EXPORT RobotTrackingSystemService : public TrackingSystemService
{
    Q_OBJECT

public:
    RobotTrackingSystemService(Ur5RobotPtr robot, VisServicesPtr services);
    virtual ~RobotTrackingSystemService();

    virtual QString getUid() const {return "org.custusx.robot.ur5";}

    virtual Tool::State getState() const;
    virtual void setState(const Tool::State val);

    virtual std::vector<ToolPtr> getTools();
    virtual TrackerConfigurationPtr getConfiguration();
    ToolPtr getReference(); /// System fails if several TrackingSystemServices define this tool

    virtual void setLoggingFolder(QString loggingFolder); ///<\param loggingFolder path to the folder where logs should be saved

signals:
    void connectToServer();
    void disconnectFromServer();

private slots:
    void configure(); ///< sets up the software
    virtual void deconfigure(); ///< deconfigures the software
    void initialize(); ///< connects to the hardware
    void uninitialize(); ///< disconnects from the hardware
    void startTracking(); ///< starts tracking
    void stopTracking(); ///< stops tracking

    void serverIsConnected();
    void serverIsDisconnected();
    void serverIsConfigured();
    void serverIsDeconfigured();

    void receiveTransform(QString devicename, Transform3D transform, double timestamp);

    void addLinksSlot();
    void removeLinksSlot();

private:
    void internalSetState(Tool::State state);
    RobotToolPtr getTool(QString devicename);

    Tool::State mState;
    std::map<QString, RobotToolPtr> mTools;
    ToolPtr mReference;

    Ur5RobotPtr mUr5Robot;
    VisServicesPtr mServices;

    RobotToolPtr mRobotTool;

    double mTimer;
    double tps;
    bool isRobotTrackingEnabled;
};

} /* namespace cx */

#endif // ROBOTTRACKINGSYSTEMSERVICE_H
