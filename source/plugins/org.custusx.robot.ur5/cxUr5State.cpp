#include "cxUr5State.h"
#include "cxUr5Receive.h"
#include "cxUr5Connection.h"


namespace cx
{

Ur5State Ur5State::updateCurrentState()
{
    return Ur5Receive::analyze_rawData(Ur5Connection::rawData);
}

Ur5State Ur5State::currentState()
{
    return this;
}

}
