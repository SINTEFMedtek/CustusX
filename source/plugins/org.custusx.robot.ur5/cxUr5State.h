#ifndef CXUR5STATE_H
#define CXUR5STATE_H

#include "org_custusx_robot_ur5_Export.h"
#include "cxVector3D.h"

namespace cx
{
struct org_custusx_robot_ur5_EXPORT Ur5State
{
    Ur5State updateCurrentState();
    Ur5State currentState();
    Vector3D cartAxis,cartAngles;
    Vector3D jointAxis,jointAngles;
    Vector3D jointAxisVelocity,jointAngleVelocity;
};


} // cx

#endif // CXUR5STATE_H
