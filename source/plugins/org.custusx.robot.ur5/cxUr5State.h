#ifndef CXUR5STATE_H
#define CXUR5STATE_H

#include "org_custusx_robot_ur5_Export.h"
#include "cxVector3D.h"

namespace cx
{

struct org_custusx_robot_ur5_EXPORT Ur5State
{
    Ur5State();
    Ur5State(double x,double y,double z,double rx,double ry,double rz);

    Vector3D cartAxis,cartAngles;
    Vector3D jointAxis,jointAngles;
    Vector3D jointAxisVelocity,jointAngleVelocity;
};


} // cx

#endif // CXUR5STATE_H
