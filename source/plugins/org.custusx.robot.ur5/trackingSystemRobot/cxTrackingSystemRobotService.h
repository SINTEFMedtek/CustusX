#ifndef CXTRACKINGSYSTEMROBOTSERVICE_H
#define CXTRACKINGSYSTEMROBOTSERVICE_H

#include "org_custusx_robot_ur5_Export.h"

#include "cxTrackingService.h"
#include "cxTrackingSystemService.h"

namespace cx
{

class org_custusx_robot_ur5_EXPORT TrackingSystemRobotService : public TrackingSystemService
{
Q_OBJECT

public:
    TrackingSystemRobotService();
    virtual ~TrackingSystemRobotService();
};

} /* namespace cx */

#endif // TRACKINGSYSTEMROBOTSERVICE_H
