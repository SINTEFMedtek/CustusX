#ifndef UR5SCRIPT_H
#define UR5SCRIPT_H

#include "org_custusx_robot_ur5_Export.h"
#include "cxUr5State.h"
#include "cxVector3D.h"

namespace cx
{

class org_custusx_robot_ur5_EXPORT Ur5Script
{
public:
    QString movec(Ur5State pose_via,Ur5State pose_to, double a, double v, double r);
    QString movej(Ur5State pose,double a, double v, double r);
    QString movel(Ur5State pose,double a, double v);

    QString speedj(double* velocityField, double a, double t);

    QString set_tcp(Ur5State pose);

    QString powerdown();
    QString set_gravity(Vector3D direction);
    QString set_payload(double mass);
    QString textmsg(QString msg);
};

} // cx

#endif // UR5SCRIPT_H
