#ifndef UR5USTRACKER_H
#define UR5USTRACKER_H


#include "org_custusx_robot_ur5_Export.h"
#include "cxUr5Robot.h"

#include <QObject>

namespace cx
{

class org_custusx_robot_ur5_EXPORT Ur5USTracker : public QObject
{
    Q_OBJECT

public:
    Ur5USTracker(Ur5RobotPtr Ur5Robot, VisServicesPtr services);
    Ur5USTracker();
    ~Ur5USTracker();

private:
    Ur5RobotPtr mUr5Robot;
    VisServicesPtr mServices;



};

} // cx

#endif // UR5USTRACKER_H
