#include "cxUr5State.h"


namespace cx
{

Ur5State::Ur5State()
{
    jointPosition = Eigen::RowVectorXd(6);
    jointVelocity = Eigen::RowVectorXd(6);
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
}

Ur5State::~Ur5State()
{
}


} // cx
