#include "cxtestIOReceiver.h"

namespace cxtest
{

IOReceiver::IOReceiver(igtlioLogicPointer logic) :
	Receiver(logic)
{

}

void IOReceiver::sendCommand_Get_Parameter_Depth()
{
	this->sendCommand_Get_Parameter("Depth");
}

void IOReceiver::sendCommand_Get_Parameter(std::string parameter_name)
{
	this->sendCommand("cx", "Get", "<command> <Parameter Name="+parameter_name+"/> </command>");
}

} //namespace cx
