#include "cxUr5USTracker.h"

#include "cxLogger.h"

#include <QTimer>

namespace cx
{

Ur5USTracker::Ur5USTracker()
{

}

Ur5USTracker::Ur5USTracker(Ur5RobotPtr Ur5Robot, VisServicesPtr services) :
    mUr5Robot(Ur5Robot),
    mServices(services)
{

}

Ur5USTracker::~Ur5USTracker()
{

}

} // cx

