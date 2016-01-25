#include "cxUr5State.h"


namespace cx
{

//Ur5State::Ur5State()
//{
//    jointConfiguration = Eigen::RowVectorXd(6);
//    jointVelocity = Eigen::RowVectorXd(6);
//    jointVelocity << 0,0,0,0,0,0;
//    jacobian = Eigen::MatrixXd(6,6);
//}

Ur5State::Ur5State(bool updt)
{
    updated = updt;

    jointConfiguration = Eigen::RowVectorXd(6);
    jointVelocity = Eigen::RowVectorXd(6);
    jacobian = Eigen::MatrixXd(6,6);
}

Ur5State::~Ur5State()
{
}

//===============================================

Ur5MovementInfo::~Ur5MovementInfo()
{

}

Ur5MovementInfo::Ur5MovementInfo():
    acceleration(0),
    velocity(0),
    time(0),
    radius(0),
    motionReference(Transform3D::Identity())
{   
    spaceFlag = undefinedSpace;
    typeOfMovement = undefinedMove;
}


} // cx
