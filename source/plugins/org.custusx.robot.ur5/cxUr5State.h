#ifndef CXUR5STATE_H
#define CXUR5STATE_H

#include "org_custusx_robot_ur5_Export.h"
#include "cxVector3D.h"
#include "cxTransform3D.h"

namespace cx
{
typedef boost::shared_ptr<struct Ur5State> Ur5StatePtr;
/**
 * Struct that holds UR5 robot information.
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
    Ur5State(bool updated);
    ~Ur5State();

    Vector3D cartAxis,cartAngles;
    //Vector3D force,torque;
    //Vector3D tcpAxis,tcpAngles;

    Eigen::RowVectorXd jointPosition;
    Eigen::RowVectorXd jointConfiguration;
    Eigen::RowVectorXd jointVelocity;
    Eigen::RowVectorXd opVelocity;

    Transform3D baseMee;

    Eigen::MatrixXd jacobian;
    Transform3D bMee;

    double timeSinceStart;

    bool updated;
};

struct org_custusx_robot_ur5_EXPORT Ur5MovementInfo
{
    Ur5MovementInfo();
    ~Ur5MovementInfo();

    QString typeOfMovement;
    QString spaceFlag;

    Eigen::RowVectorXd targetJointConfiguration;
    Eigen::RowVectorXd targetSpatialConfiguration;

    Eigen::RowVectorXd targetJointVelocity;
    Eigen::RowVectorXd targetSpatialVelocity;

    double acceleration;
    double velocity;
    double radius;
    double time;

    bool isJointMove();
};




} // cx

#endif // CXUR5STATE_H
