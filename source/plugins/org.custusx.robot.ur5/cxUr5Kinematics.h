#ifndef UR5KINEMATICS_H
#define UR5KINEMATICS_H

#include "org_custusx_robot_ur5_Export.h"
#include "cxVector3D.h"
#include "cxTransform3D.h"



namespace cx
{
#define ZERO_THRESH 0.00000001
#define SIGN(x) ( ( (x) > 0 ) - ( (x) < 0 ) )
#define PI M_PI

class org_custusx_robot_ur5_EXPORT Ur5Kinematics
{
public:
    Eigen::MatrixXd forward(Eigen::RowVectorXd jointConfiguration);
    Eigen::MatrixXd forward2(Eigen::RowVectorXd jointConfiguration);
    Eigen::MatrixXd jacobian(Eigen::RowVectorXd jointConfiguration);

    Eigen::MatrixXd inverse(Eigen::MatrixXd operationalConfiguration);
    Eigen::RowVectorXd inverseJ(Eigen::MatrixXd desiredPose, Eigen::RowVectorXd guessedJointConfiguration);

    Transform3D poseToMatrix(Eigen::RowVectorXd poseConfiguration);

    Vector3D T2transl(Eigen::MatrixXd T);
    Vector3D T2transl(Transform3D T);

    Eigen::RowVectorXd errorVector(Eigen::MatrixXd desiredPose, Eigen::MatrixXd currentPose);
private:
    double d1 = 0.089159;
    double a2 = -0.42500;
    double a3 = -0.39225;
    double d4 = 0.10915;
    double d5 = 0.09465;
    double d6 = 0.0823;
};

} // cx

#endif // UR5KINEMATICS_H
