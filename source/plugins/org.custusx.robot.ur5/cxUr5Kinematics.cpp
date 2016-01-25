#include "cxUr5Kinematics.h"

namespace cx
{

Transform3D Ur5Kinematics::forward(Eigen::RowVectorXd jointConfiguration)
{
    Eigen::Matrix4d matrix;

    double s1 = sin(jointConfiguration(0)), c1 = cos(jointConfiguration(0));
    double s2 = sin(jointConfiguration(1)), c2 = cos(jointConfiguration(1));
    double s3 = sin(jointConfiguration(2)), c3 = cos(jointConfiguration(2));
    double s4 = sin(jointConfiguration(3)), c4 = cos(jointConfiguration(3));
    double s5 = sin(jointConfiguration(4)), c5 = cos(jointConfiguration(4));
    double s6 = sin(jointConfiguration(5)), c6 = cos(jointConfiguration(5));

    double s23 = sin(jointConfiguration(1)+jointConfiguration(2));
    double c23 = cos(jointConfiguration(1)+jointConfiguration(2));
    double s234 = sin(jointConfiguration(1)+jointConfiguration(2)+jointConfiguration(3));
    double c234 = cos(jointConfiguration(1)+jointConfiguration(2)+jointConfiguration(3));

    matrix << c6*(s1*s5 + c234*c1*c5) - s234*c1*s6,
            - s6*(s1*s5 + c234*c1*c5) - s234*c1*c6,
            c5*s1 - c234*c1*s5,
            d6*(c5*s1 - c234*c1*s5) + d4*s1 + a2*c1*c2 + d5*s234*c1 + a3*c1*c2*c3 - a3*c1*s2*s3,
            - c6*(c1*s5 - c234*c5*s1) - s234*s1*s6,
            s6*(c1*s5 - c234*c5*s1) - s234*c6*s1,
            - c1*c5 - c234*s1*s5,
            a2*c2*s1 - d4*c1 - d6*(c1*c5 + c234*s1*s5) + d5*s234*s1 + a3*c2*c3*s1 - a3*s1*s2*s3,
            c234*s6 + s234*c5*c6,
            c234*c6 - s234*c5*s6,
            -s234*s5,
            d1 + d5*(s23*s4 - c23*c4) + a3*s23 + a2*s2 - d6*s5*(c23*s4 + s23*c4),
            0, 0, 0, 1;

    return Eigen::Affine3d(matrix);
}

Eigen::MatrixXd Ur5Kinematics::jacobian2(Eigen::RowVectorXd jointConfiguration)
{
    Eigen::RowVectorXd s(6), c(6);

    s << sin(jointConfiguration(0)), sin(jointConfiguration(1)), sin(jointConfiguration(2)),
            sin(jointConfiguration(3)), sin(jointConfiguration(4)), sin(jointConfiguration(5));

    c << cos(jointConfiguration(0)), cos(jointConfiguration(1)), cos(jointConfiguration(2)),
            cos(jointConfiguration(3)), cos(jointConfiguration(4)), cos(jointConfiguration(5));


    Eigen::MatrixXd matrix(6,6);

    matrix <<   (2183*c(0))/20000 + (823*c(0)*c(4))/10000 + (17*c(1)*s(0))/40 - (1893*c(3)*(c(1)*s(0)*s(2) + c(2)*s(0)*s(1)))/20000 + (1893*s(3)*(s(0)*s(1)*s(2) - c(1)*c(2)*s(0)))/20000 - (823*s(4)*(c(3)*(s(0)*s(1)*s(2) - c(1)*c(2)*s(0)) + s(3)*(c(1)*s(0)*s(2) + c(2)*s(0)*s(1))))/10000 - (157*s(0)*s(1)*s(2))/400 + (157*c(1)*c(2)*s(0))/400,
                c(0)*((17*s(1))/40 + (157*c(1)*s(2))/400 + (157*c(2)*s(1))/400 + (823*s(4)*(c(3)*(c(1)*s(2) + c(2)*s(1)) + s(3)*(c(1)*c(2) - s(1)*s(2))))/10000 + (1893*c(3)*(c(1)*c(2) - s(1)*s(2)))/20000 - (1893*s(3)*(c(1)*s(2) + c(2)*s(1)))/20000),
                c(0)*((157*c(1)*s(2))/400 + (157*c(2)*s(1))/400 + (823*s(4)*(c(3)*(c(1)*s(2) + c(2)*s(1)) + s(3)*(c(1)*c(2) - s(1)*s(2))))/10000 + (1893*c(3)*(c(1)*c(2) - s(1)*s(2)))/20000 - (1893*s(3)*(c(1)*s(2) + c(2)*s(1)))/20000),
                c(0)*((823*s(4)*(c(3)*(c(1)*s(2) + c(2)*s(1)) + s(3)*(c(1)*c(2) - s(1)*s(2))))/10000 + (1893*c(3)*(c(1)*c(2) - s(1)*s(2)))/20000 - (1893*s(3)*(c(1)*s(2) + c(2)*s(1)))/20000),
                -(c(3)*(c(1)*c(2) - s(1)*s(2)) - s(3)*(c(1)*s(2) + c(2)*s(1)))*((823*c(0)*c(4))/10000 - (1893*c(3)*(c(1)*s(0)*s(2) + c(2)*s(0)*s(1)))/20000 + (1893*s(3)*(s(0)*s(1)*s(2) - c(1)*c(2)*s(0)))/20000 - (823*s(4)*(c(3)*(s(0)*s(1)*s(2) - c(1)*c(2)*s(0)) + s(3)*(c(1)*s(0)*s(2) + c(2)*s(0)*s(1))))/10000) - (c(3)*(c(1)*s(0)*s(2) + c(2)*s(0)*s(1)) - s(3)*(s(0)*s(1)*s(2) - c(1)*c(2)*s(0)))*((823*s(4)*(c(3)*(c(1)*s(2) + c(2)*s(1)) + s(3)*(c(1)*c(2) - s(1)*s(2))))/10000 + (1893*c(3)*(c(1)*c(2) - s(1)*s(2)))/20000 - (1893*s(3)*(c(1)*s(2) + c(2)*s(1)))/20000),
                (823*s(4)*(c(3)*(c(1)*s(2) + c(2)*s(1)) + s(3)*(c(1)*c(2) - s(1)*s(2)))*(c(0)*c(4) - s(4)*(c(3)*(s(0)*s(1)*s(2) - c(1)*c(2)*s(0)) + s(3)*(c(1)*s(0)*s(2) + c(2)*s(0)*s(1)))))/10000 - s(4)*(c(3)*(c(1)*s(2) + c(2)*s(1)) + s(3)*(c(1)*c(2) - s(1)*s(2)))*((823*c(0)*c(4))/10000 - (823*s(4)*(c(3)*(s(0)*s(1)*s(2) - c(1)*c(2)*s(0)) + s(3)*(c(1)*s(0)*s(2) + c(2)*s(0)*s(1))))/10000),
                (2183*s(0))/20000 - (17*c(0)*c(1))/40 + (823*c(4)*s(0))/10000 + (1893*c(3)*(c(0)*c(1)*s(2) + c(0)*c(2)*s(1)))/20000 - (1893*s(3)*(c(0)*s(1)*s(2) - c(0)*c(1)*c(2)))/20000 + (823*s(4)*(c(3)*(c(0)*s(1)*s(2) - c(0)*c(1)*c(2)) + s(3)*(c(0)*c(1)*s(2) + c(0)*c(2)*s(1))))/10000 + (157*c(0)*s(1)*s(2))/400 - (157*c(0)*c(1)*c(2))/400,
                s(0)*((17*s(1))/40 + (157*c(1)*s(2))/400 + (157*c(2)*s(1))/400 + (823*s(4)*(c(3)*(c(1)*s(2) + c(2)*s(1)) + s(3)*(c(1)*c(2) - s(1)*s(2))))/10000 + (1893*c(3)*(c(1)*c(2) - s(1)*s(2)))/20000 - (1893*s(3)*(c(1)*s(2) + c(2)*s(1)))/20000),
                s(0)*((157*c(1)*s(2))/400 + (157*c(2)*s(1))/400 + (823*s(4)*(c(3)*(c(1)*s(2) + c(2)*s(1)) + s(3)*(c(1)*c(2) - s(1)*s(2))))/10000 + (1893*c(3)*(c(1)*c(2) - s(1)*s(2)))/20000 - (1893*s(3)*(c(1)*s(2) + c(2)*s(1)))/20000),
                s(0)*((823*s(4)*(c(3)*(c(1)*s(2) + c(2)*s(1)) + s(3)*(c(1)*c(2) - s(1)*s(2))))/10000 + (1893*c(3)*(c(1)*c(2) - s(1)*s(2)))/20000 - (1893*s(3)*(c(1)*s(2) + c(2)*s(1)))/20000),
                (c(3)*(c(0)*c(1)*s(2) + c(0)*c(2)*s(1)) - s(3)*(c(0)*s(1)*s(2) - c(0)*c(1)*c(2)))*((823*s(4)*(c(3)*(c(1)*s(2) + c(2)*s(1)) + s(3)*(c(1)*c(2) - s(1)*s(2))))/10000 + (1893*c(3)*(c(1)*c(2) - s(1)*s(2)))/20000 - (1893*s(3)*(c(1)*s(2) + c(2)*s(1)))/20000) - (c(3)*(c(1)*c(2) - s(1)*s(2)) - s(3)*(c(1)*s(2) + c(2)*s(1)))*((823*c(4)*s(0))/10000 + (1893*c(3)*(c(0)*c(1)*s(2) + c(0)*c(2)*s(1)))/20000 - (1893*s(3)*(c(0)*s(1)*s(2) - c(0)*c(1)*c(2)))/20000 + (823*s(4)*(c(3)*(c(0)*s(1)*s(2) - c(0)*c(1)*c(2)) + s(3)*(c(0)*c(1)*s(2) + c(0)*c(2)*s(1))))/10000),
                (823*s(4)*(c(3)*(c(1)*s(2) + c(2)*s(1)) + s(3)*(c(1)*c(2) - s(1)*s(2)))*(c(4)*s(0) + s(4)*(c(3)*(c(0)*s(1)*s(2) - c(0)*c(1)*c(2)) + s(3)*(c(0)*c(1)*s(2) + c(0)*c(2)*s(1)))))/10000 - s(4)*(c(3)*(c(1)*s(2) + c(2)*s(1)) + s(3)*(c(1)*c(2) - s(1)*s(2)))*((823*c(4)*s(0))/10000 + (823*s(4)*(c(3)*(c(0)*s(1)*s(2) - c(0)*c(1)*c(2)) + s(3)*(c(0)*c(1)*s(2) + c(0)*c(2)*s(1))))/10000),
                0,
                c(0)*((2183*s(0))/20000 - (17*c(0)*c(1))/40 + (823*c(4)*s(0))/10000 + (1893*c(3)*(c(0)*c(1)*s(2) + c(0)*c(2)*s(1)))/20000 - (1893*s(3)*(c(0)*s(1)*s(2) - c(0)*c(1)*c(2)))/20000 + (823*s(4)*(c(3)*(c(0)*s(1)*s(2) - c(0)*c(1)*c(2)) + s(3)*(c(0)*c(1)*s(2) + c(0)*c(2)*s(1))))/10000 + (157*c(0)*s(1)*s(2))/400 - (157*c(0)*c(1)*c(2))/400) - s(0)*((2183*c(0))/20000 + (823*c(0)*c(4))/10000 + (17*c(1)*s(0))/40 - (1893*c(3)*(c(1)*s(0)*s(2) + c(2)*s(0)*s(1)))/20000 + (1893*s(3)*(s(0)*s(1)*s(2) - c(1)*c(2)*s(0)))/20000 - (823*s(4)*(c(3)*(s(0)*s(1)*s(2) - c(1)*c(2)*s(0)) + s(3)*(c(1)*s(0)*s(2) + c(2)*s(0)*s(1))))/10000 - (157*s(0)*s(1)*s(2))/400 + (157*c(1)*c(2)*s(0))/400),
                c(0)*((2183*s(0))/20000 + (823*c(4)*s(0))/10000 + (1893*c(3)*(c(0)*c(1)*s(2) + c(0)*c(2)*s(1)))/20000 - (1893*s(3)*(c(0)*s(1)*s(2) - c(0)*c(1)*c(2)))/20000 + (823*s(4)*(c(3)*(c(0)*s(1)*s(2) - c(0)*c(1)*c(2)) + s(3)*(c(0)*c(1)*s(2) + c(0)*c(2)*s(1))))/10000 + (157*c(0)*s(1)*s(2))/400 - (157*c(0)*c(1)*c(2))/400) - s(0)*((2183*c(0))/20000 + (823*c(0)*c(4))/10000 - (1893*c(3)*(c(1)*s(0)*s(2) + c(2)*s(0)*s(1)))/20000 + (1893*s(3)*(s(0)*s(1)*s(2) - c(1)*c(2)*s(0)))/20000 - (823*s(4)*(c(3)*(s(0)*s(1)*s(2) - c(1)*c(2)*s(0)) + s(3)*(c(1)*s(0)*s(2) + c(2)*s(0)*s(1))))/10000 - (157*s(0)*s(1)*s(2))/400 + (157*c(1)*c(2)*s(0))/400),
                c(0)*((2183*s(0))/20000 + (823*c(4)*s(0))/10000 + (1893*c(3)*(c(0)*c(1)*s(2) + c(0)*c(2)*s(1)))/20000 - (1893*s(3)*(c(0)*s(1)*s(2) - c(0)*c(1)*c(2)))/20000 + (823*s(4)*(c(3)*(c(0)*s(1)*s(2) - c(0)*c(1)*c(2)) + s(3)*(c(0)*c(1)*s(2) + c(0)*c(2)*s(1))))/10000) - s(0)*((2183*c(0))/20000 + (823*c(0)*c(4))/10000 - (1893*c(3)*(c(1)*s(0)*s(2) + c(2)*s(0)*s(1)))/20000 + (1893*s(3)*(s(0)*s(1)*s(2) - c(1)*c(2)*s(0)))/20000 - (823*s(4)*(c(3)*(s(0)*s(1)*s(2) - c(1)*c(2)*s(0)) + s(3)*(c(1)*s(0)*s(2) + c(2)*s(0)*s(1))))/10000),
                -(c(3)*(c(1)*s(0)*s(2) + c(2)*s(0)*s(1)) - s(3)*(s(0)*s(1)*s(2) - c(1)*c(2)*s(0)))*((823*c(4)*s(0))/10000 + (1893*c(3)*(c(0)*c(1)*s(2) + c(0)*c(2)*s(1)))/20000 - (1893*s(3)*(c(0)*s(1)*s(2) - c(0)*c(1)*c(2)))/20000 + (823*s(4)*(c(3)*(c(0)*s(1)*s(2) - c(0)*c(1)*c(2)) + s(3)*(c(0)*c(1)*s(2) + c(0)*c(2)*s(1))))/10000) - (c(3)*(c(0)*c(1)*s(2) + c(0)*c(2)*s(1)) - s(3)*(c(0)*s(1)*s(2) - c(0)*c(1)*c(2)))*((823*c(0)*c(4))/10000 - (1893*c(3)*(c(1)*s(0)*s(2) + c(2)*s(0)*s(1)))/20000 + (1893*s(3)*(s(0)*s(1)*s(2) - c(1)*c(2)*s(0)))/20000 - (823*s(4)*(c(3)*(s(0)*s(1)*s(2) - c(1)*c(2)*s(0)) + s(3)*(c(1)*s(0)*s(2) + c(2)*s(0)*s(1))))/10000),
                ((823*c(4)*s(0))/10000 + (823*s(4)*(c(3)*(c(0)*s(1)*s(2) - c(0)*c(1)*c(2)) + s(3)*(c(0)*c(1)*s(2) + c(0)*c(2)*s(1))))/10000)*(c(0)*c(4) - s(4)*(c(3)*(s(0)*s(1)*s(2) - c(1)*c(2)*s(0)) + s(3)*(c(1)*s(0)*s(2) + c(2)*s(0)*s(1)))) - (c(4)*s(0) + s(4)*(c(3)*(c(0)*s(1)*s(2) - c(0)*c(1)*c(2)) + s(3)*(c(0)*c(1)*s(2) + c(0)*c(2)*s(1))))*((823*c(0)*c(4))/10000 - (823*s(4)*(c(3)*(s(0)*s(1)*s(2) - c(1)*c(2)*s(0)) + s(3)*(c(1)*s(0)*s(2) + c(2)*s(0)*s(1))))/10000),
                0,
                s(0),
                s(0),
                s(0),
                c(3)*(c(0)*c(1)*s(2) + c(0)*c(2)*s(1)) - s(3)*(c(0)*s(1)*s(2) - c(0)*c(1)*c(2)),
                c(4)*s(0) + s(4)*(c(3)*(c(0)*s(1)*s(2) - c(0)*c(1)*c(2)) + s(3)*(c(0)*c(1)*s(2) + c(0)*c(2)*s(1))),
                0,
                -c(0),
                -c(0),
                -c(0),
                c(3)*(c(1)*s(0)*s(2) + c(2)*s(0)*s(1)) - s(3)*(s(0)*s(1)*s(2) - c(1)*c(2)*s(0)),
                s(4)*(c(3)*(s(0)*s(1)*s(2) - c(1)*c(2)*s(0)) + s(3)*(c(1)*s(0)*s(2) + c(2)*s(0)*s(1))) - c(0)*c(4),
                1,
                0,
                0,
                0,
                s(3)*(c(1)*s(2) + c(2)*s(1)) - c(3)*(c(1)*c(2) - s(1)*s(2)),
                -s(4)*(c(3)*(c(1)*s(2) + c(2)*s(1)) + s(3)*(c(1)*c(2) - s(1)*s(2)));

    return matrix;
}

Eigen::MatrixXd Ur5Kinematics::jacobian(Eigen::RowVectorXd jointConfiguration)
{
    double s1 = sin(jointConfiguration(0)), c1 = cos(jointConfiguration(0));
    double s2 = sin(jointConfiguration(1)), c2 = cos(jointConfiguration(1));
    double s3 = sin(jointConfiguration(2)), c3 = cos(jointConfiguration(2));
    double s4 = sin(jointConfiguration(3)), c4 = cos(jointConfiguration(3));
    double s5 = sin(jointConfiguration(4)), c5 = cos(jointConfiguration(4));
    double s6 = sin(jointConfiguration(5)), c6 = cos(jointConfiguration(5));

    double s23 = sin(jointConfiguration(1)+jointConfiguration(2));
    double c23 = cos(jointConfiguration(1)+jointConfiguration(2));
    double s234 = sin(jointConfiguration(1)+jointConfiguration(2)+jointConfiguration(3));
    double c234 = cos(jointConfiguration(1)+jointConfiguration(2)+jointConfiguration(3));
    double s2345 = sin(jointConfiguration(1)+jointConfiguration(2)+jointConfiguration(3)+jointConfiguration(4));
    double s234m5 = sin(jointConfiguration(1)+jointConfiguration(2)+jointConfiguration(3)-jointConfiguration(4));


    Eigen::MatrixXd matrix(6,6);

    matrix << d6*(c1*c5 + c234*s1*s5) + d4*c1 - a2*c2*s1 - d5*s234*s1 - a3*c2*c3*s1 + a3*s1*s2*s3,
              -c1*(d5*(s23*s4 - c23*c4) + a3*s23 + a2*s2 - d6*s5*(c23*s4 + s23*c4)),
              c1*(d5*c234 - a3*s23 + d6*s234*s5),
              c1*(d5*c234 + d6*s234*s5),
              d6*c1*c2*c5*s3*s4 - d6*s1*s5 + d6*c1*c3*c5*s2*s4 + d6*c1*c4*c5*s2*s3 - d6*c1*c2*c3*c4*c5,
              0,
              d6*(c5*s1 - c234*c1*s5) + d4*s1 + a2*c1*c2 + d5*s234*c1 + a3*c1*c2*c3 - a3*c1*s2*s3,
              -s1*(d5*(s23*s4 - c23*c4) + a3*s23 + a2*s2 - d6*s5*(c23*s4 + s23*c4)),
              s1*(d5*c234 - a3*s23 + d6*s234*s5),
              s1*(d5*c234 + d6*s234*s5),
              d6*c1*s5 - d6*c2*c3*c4*c5*s1 + d6*c2*c5*s1*s3*s4 + d6*c3*c5*s1*s2*s4 + d6*c4*c5*s1*s2*s3,
              0,
              0,
              a3*c23 - (d6*s2345)/2 + a2*c2 + (d6*s234m5)/2 + d5*s234,
              a3*c23 - (d6*s2345)/2 + (d6*s234m5)/2 + d5*s234,
              (d6*s234m5)/2 - (d6*s2345)/2 + d5*s234,
              -d6*(s234m5/2 + s2345/2),
              0,
              0,
              s1,
              s1,
              s1,
              s234*c1,
              c5*s1 - c234*c1*s5,
              0,
              -c1,
              -c1,
              -c1,
              s234*s1,
              - c1*c5 - c234*s1*s5,
              1,
              0,
              0,
              0,
              -c234,
              -s234*s5;


    return matrix;
}

Transform3D Ur5Kinematics::poseToMatrix(Eigen::RowVectorXd poseConfiguration)
{
    Transform3D matrix;

    Vector3D cartAngles;
    cartAngles << poseConfiguration(3), poseConfiguration(4), poseConfiguration(5);

    Vector3D cartAxis;
    cartAxis << poseConfiguration(0), poseConfiguration(1), poseConfiguration(2);

    matrix = Eigen::AngleAxisd(cartAngles.norm(),cartAngles/cartAngles.norm());
    matrix.translation() = cartAxis*1000;

    return matrix;
}

Vector3D Ur5Kinematics::T2transl(Eigen::MatrixXd T)
{
    Vector3D points;

    points << T(0,3), T(1,3), T(2,3);
    return points;
}

Vector3D Ur5Kinematics::T2transl(Transform3D T)
{
    Vector3D points;

    points << T(0,3), T(1,3), T(2,3);
    return points;
}

Vector3D Ur5Kinematics::T2rangles(Transform3D T)
{
    double angle;
    Vector3D k;

    angle = acos((T(0,0)+T(1,1)+T(2,2)-1)/2);
    k << (T(2,1)-T(1,2)),(T(0,2)-T(2,0)),(T(1,0)-T(0,1));
    k = (1/(2*sin(angle)))*k*angle;

    return k;
}

Eigen::RowVectorXd Ur5Kinematics::inverseJ(Transform3D desiredPose, Eigen::RowVectorXd guessedJointConfiguration)
{
    double errorThreshold = 10^-6;
    double K = 1;

    Eigen::RowVectorXd jointConfiguration(6),error(6),dq(6);
    jointConfiguration = guessedJointConfiguration;

    Eigen::MatrixXd jacobi(6,6);
    Transform3D currentPose;

    currentPose = forward(jointConfiguration);
    error = errorVector(desiredPose,currentPose);

    while(error.length()>errorThreshold)
    {
        jacobi = jacobian(jointConfiguration);
        currentPose = forward(jointConfiguration);

        dq = K*pseudoInverse(jacobi)*errorVector(desiredPose,currentPose);
        jointConfiguration = jointConfiguration + dq;

        error = errorVector(desiredPose,currentPose);
    }

    return jointConfiguration;
}

Eigen::RowVectorXd Ur5Kinematics::errorVector(Transform3D desiredPose, Transform3D currentPose) const
{
    Eigen::RowVectorXd e(6);

    e << desiredPose(0,3)-currentPose(0,3),desiredPose(1,3)-currentPose(1,3), desiredPose(2,3)-currentPose(2,3);

    Eigen::MatrixXd Rqd, Rqe, R;
    Rqd = getRotation(desiredPose);
    Rqe = getRotation(currentPose);

    R = Rqd*Rqe.transpose();

    e << 0.5*(R(2,1)-R(1,2)), 0.5*(R(0,2)-R(2,0)), 0.5*(R(1,0)-R(0,1));

    return e;
}

Eigen::MatrixXd Ur5Kinematics::getRotation(Transform3D pose) const
{
    return pose.rotation();
}

Eigen::MatrixXd Ur5Kinematics::pseudoInverse(Eigen::MatrixXd matrix) const
{
    return matrix.transpose()*(matrix*matrix.transpose()).inverse();
}


Transform3D Ur5Kinematics::T01(Eigen::RowVectorXd jointConfiguration)
{
    Eigen::Matrix4d matrix;

    double s1 = sin(jointConfiguration(0)), c1 = cos(jointConfiguration(0));

    matrix << c1, 0,  s1,  0,
              s1, 0, -c1,  0,
               0, 1,   0,  d1*1000,
               0, 0,   0,   1;

    return Eigen::Affine3d(matrix);
}

Transform3D Ur5Kinematics::T02(Eigen::RowVectorXd jointConfiguration)
{
    Eigen::Matrix4d matrix;

    double s1 = sin(jointConfiguration(0)), c1 = cos(jointConfiguration(0));
    double s2 = sin(jointConfiguration(1)), c2 = cos(jointConfiguration(1));

    matrix << c1*c2, -c1*s2,  s1, (a2*c1*c2)*1000,
              c2*s1, -s1*s2, -c1, (a2*c2*s1)*1000,
              s2,        c2,   0, (d1 + a2*s2)*1000,
               0,         0,   0,          1;

    return Eigen::Affine3d(matrix);
}

Transform3D Ur5Kinematics::T03(Eigen::RowVectorXd jointConfiguration)
{
    Eigen::Matrix4d matrix;

    double s1 = sin(jointConfiguration(0)), c1 = cos(jointConfiguration(0));
    double s2 = sin(jointConfiguration(1)), c2 = cos(jointConfiguration(1));
    double s23 = sin(jointConfiguration(1)+jointConfiguration(2));
    double c23 = cos(jointConfiguration(1)+jointConfiguration(2));

    matrix << c23*c1, -s23*c1,  s1,     (c1*(a3*c23+a2*c2))*1000,
              c23*s1, -s23*s1, -c1,     (s1*(a3*c23+a2*c2))*1000,
                 s23,     c23,   0,      (d1+a3*s23+a2*s2)*1000,
                   0,       0,   0,                     1;

    return Eigen::Affine3d(matrix);
}

Transform3D Ur5Kinematics::T04(Eigen::RowVectorXd jointConfiguration)
{
    Eigen::Matrix4d matrix;

    double s1 = sin(jointConfiguration(0)), c1 = cos(jointConfiguration(0));
    double s2 = sin(jointConfiguration(1)), c2 = cos(jointConfiguration(1));
    double s3 = sin(jointConfiguration(2)), c3 = cos(jointConfiguration(2));

    double s23 = sin(jointConfiguration(1)+jointConfiguration(2));
    double s234 = sin(jointConfiguration(1)+jointConfiguration(2)+jointConfiguration(3));
    double c234 = cos(jointConfiguration(1)+jointConfiguration(2)+jointConfiguration(3));


    matrix << c234*c1,  s1,     s234*c1,    (d4*s1 + a2*c1*c2 + a3*c1*c2*c3 - a3*c1*s2*s3)*1000,
              c234*s1, -c1,     s234*s1,    (a2*c2*s1 - d4*c1 + a3*c2*c3*s1 - a3*s1*s2*s3)*1000,
              s234,      0,       -c234,   (d1 + a3*s23 + a2*s2)*1000,
              0,         0,                   0,                                1;

    return Eigen::Affine3d(matrix);
}

Transform3D Ur5Kinematics::T05(Eigen::RowVectorXd jointConfiguration)
{
    Eigen::Matrix4d matrix;

    double s1 = sin(jointConfiguration(0)), c1 = cos(jointConfiguration(0));
    double s2 = sin(jointConfiguration(1)), c2 = cos(jointConfiguration(1));
    double s3 = sin(jointConfiguration(2)), c3 = cos(jointConfiguration(2));
    double s4 = sin(jointConfiguration(3)), c4 = cos(jointConfiguration(3));
    double s5 = sin(jointConfiguration(4)), c5 = cos(jointConfiguration(4));

    double s23 = sin(jointConfiguration(1)+jointConfiguration(2));
    double s234 = sin(jointConfiguration(1)+jointConfiguration(2)+jointConfiguration(3));
    double c234 = cos(jointConfiguration(1)+jointConfiguration(2)+jointConfiguration(3));


    matrix << s1*s5 + c234*c1*c5, -s234*c1,   c5*s1 - c234*c1*s5, (d5*(c4*(c1*c2*s3 + c1*c3*s2) - s4*(c1*s2*s3 - c1*c2*c3)) + d4*s1 + a2*c1*c2 + a3*c1*c2*c3 - a3*c1*s2*s3)*1000,
              c234*c5*s1 - c1*s5, -s234*s1, - c1*c5 - c234*s1*s5, (d5*(c4*(c2*s1*s3 + c3*s1*s2) - s4*(s1*s2*s3 - c2*c3*s1)) - d4*c1 + a2*c2*s1 + a3*c2*c3*s1 - a3*s1*s2*s3)*1000,
              s234*c5,           c234,      -s234*s5,             (d1 + a3*s23 + a2*s2 - d5*c234)*1000,
              0,                              0, 0, 1;

    return Eigen::Affine3d(matrix);
}

} // cx
