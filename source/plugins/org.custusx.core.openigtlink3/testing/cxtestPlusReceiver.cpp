#include "cxtestPlusReceiver.h"

namespace cxtest
{

PlusReceiver::PlusReceiver(igtlio::LogicPointer logic) :
	Receiver(logic)
{

}

void PlusReceiver::send_RequestChannelIDs()
{
	/* PLUS commands
	static const char REQUEST_CHANNEL_IDS_CMD[] = "RequestChannelIds";
	static const char REQUEST_DEVICE_CHANNEL_IDS_CMD[] = "RequestDeviceChannelIds";
	static const char REQUEST_DEVICE_IDS_CMD[] = "RequestDeviceIds";
	static const char REQUEST_INPUT_DEVICE_IDS_CMD[] = "RequestInputDeviceIds";
	 */
	this->sendCommand("", "RequestChannelIds", "<Command Name=\"RequestChannelIds\" />");

	//Reply should be: <Command><Result>true</Result><Message>VideoStream</Message></Command>
}

void PlusReceiver::send_RequestDeviceIds()
{
	this->sendCommand("", "RequestDeviceIds", "<Command Name=\"RequestDeviceIds\" />");

	//Reply should be: <Command><Result>true</Result><Message>VideoDevice,CaptureDevice</Message></Command>
}

void PlusReceiver::send_RequestDepthAndGain()
{
	this->sendCommand("VideoDevice", "Get", "<Command Name=\"Get\" > <parameter name=\"Depth\"/> <parameter name=\"Gain\"/> </Command>");
}


} //namespace cxtest

