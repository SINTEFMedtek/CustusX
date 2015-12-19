#include "cxUr5State.h"


namespace cx
{

Ur5State::Ur5State()
{
    jointPosition = Eigen::RowVectorXd(6);
    jointVelocity = Eigen::RowVectorXd(6);
    jointVelocity << 0,0,0,0,0,0;
    baseMee = Transform3D::Identity();
    jacobian = Eigen::MatrixXd(6,6);
}

Ur5State::Ur5State(double x,double y,double z,double rx,double ry,double rz)
{
    cartAxis << x, y, z;
    cartAngles << rx,ry,rz;

    jointPosition = Eigen::RowVectorXd(6);
    jointVelocity = Eigen::RowVectorXd(6);
}

Ur5State::Ur5State(bool updt)
{
    updated = updt;

    jointPosition = Eigen::RowVectorXd(6);
    jointVelocity = Eigen::RowVectorXd(6);
    baseMee = Transform3D::Identity();
}

Ur5State::~Ur5State()
{
}
//===============================================

Ur5MovementInfo::~Ur5MovementInfo()
{

}

Ur5MovementInfo::Ur5MovementInfo()
{
    typeOfMovement = "undefined";
    acceleration = 0;
    velocity = 0;
    time = 0;
    radius = 0;
    spaceFlag = "undefined";
}

bool Ur5MovementInfo::isJointMove()
{
    if(spaceFlag == "jointConfiguration")
        return true;
    return false;
}


} // cx
