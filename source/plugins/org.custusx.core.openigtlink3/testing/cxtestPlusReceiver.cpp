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
	this->sendCommand("cx", "RequestDeviceIds", "<Command Name=\"RequestChannelIDs\" />");
}

} //namespace cxtest

