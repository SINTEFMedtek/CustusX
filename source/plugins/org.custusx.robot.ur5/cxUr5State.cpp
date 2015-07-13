#include "cxUr5State.h"
#include "cxUr5Receive.h"
#include "cxUr5Connection.h"


namespace cx
{

Ur5State::Ur5State()
{

}

Ur5State::Ur5State(double x,double y,double z,double rx,double ry,double rz)
{
    cartAxis << x, y, z;
    cartAngles << rx,ry,rz;
}

} // cx
