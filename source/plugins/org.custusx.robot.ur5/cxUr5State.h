#ifndef CXUR5STATE_H
#define CXUR5STATE_H

#include "org_custusx_robot_ur5_Export.h"
#include "cxVector3D.h"

namespace cx
{
typedef boost::shared_ptr<struct Ur5State> Ur5StatePtr;
/**
 * Struct that handles UR5 robot spatial information.
 *
 * \ingroup org_custusx_robot_ur5
 *
 * \author Andreas Østvik
 * \date 2015-07-10
 */


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
