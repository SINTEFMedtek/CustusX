#ifndef UR5MESSAGEENCODER_H
#define UR5MESSAGEENCODER_H

#include "org_custusx_robot_ur5_Export.h"
#include "cxUr5State.h"
#include "cxVector3D.h"

namespace cx
{

class org_custusx_robot_ur5_EXPORT Ur5MessageEncoder
{
public:
    QString movec(Ur5State pose_via,Ur5State pose_to, double a, double v, double r);
    QString movej(Ur5State pose,double a, double v, double r);
    QString movej(Eigen::RowVectorXd pose, double a, double v, double t, double r);
    QString movej(Eigen::RowVectorXd pose, double t);
    QString movej(Ur5MovementInfo movementInformation);

    QString movel(Ur5State pose,double a, double v);

    QString speedj(Ur5State p, double a, double t);
    QString speedj(Eigen::RowVectorXd p, double a, double t);
    QString speedj(Ur5MovementInfo movementInformation);

    QString speedl(Ur5State p, double a, double t);
    QString speedl(Eigen::RowVectorXd p, double a, double t);

    QString stopj(double a);
    QString stopl(double a);

    QString set_tcp(Ur5State pose);

    QString powerdown();
    QString set_gravity(Vector3D direction);
    QString set_payload(double mass);

    QString textmsg(QString msg);
    QString sleep(double time);
};

} // cx

#endif // UR5MESSAGEENCODER_H
