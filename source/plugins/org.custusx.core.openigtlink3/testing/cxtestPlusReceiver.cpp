#include "cxtestPlusReceiver.h"

namespace cxtest
{

PlusReceiver::PlusReceiver(igtlioLogicPointer logic) :
	Receiver(logic)
{

}

//Replies to these messages are sent as igtl::RTSCommandMessage

void PlusReceiver::send_RequestChannelIds()
{
	/* Plus commands
	static const char REQUEST_CHANNEL_IDS_CMD[] = "RequestChannelIds";
	static const char REQUEST_DEVICE_CHANNEL_IDS_CMD[] = "RequestDeviceChannelIds";
	static const char REQUEST_DEVICE_IDS_CMD[] = "RequestDeviceIds";
	static const char REQUEST_INPUT_DEVICE_IDS_CMD[] = "RequestInputDeviceIds";
	 */
	this->sendCommand("", "RequestChannelIds", "<Command Name=\"RequestChannelIds\" />");

	//Reply should be: <Command><Result>true</Result><Message>VideoStream</Message></Command>
}

void PlusReceiver::send_RequestDeviceChannelIds()
{
	this->sendCommand("VideoDevice", "RequestDeviceChannelIds", "<Command Name=\"RequestDeviceChannelIds\" DeviceId=\"VideoDevice\" />");

	//Reply should be: <Command><Result>true</Result><Message>VideoStream,</Message></Command>
}

void PlusReceiver::send_RequestDeviceIds()
{
	this->sendCommand("", "RequestDeviceIds", "<Command Name=\"RequestDeviceIds\" />");

	//Reply should be: <Command><Result>true</Result><Message>VideoDevice,CaptureDevice</Message></Command>
}

void PlusReceiver::send_RequestInputDeviceIds()
{
	this->sendCommand("VideoDevice", "RequestInputDeviceIds", "<Command Name=\"RequestInputDeviceIds\" DeviceId=\"VideoDevice\" />");
}

void PlusReceiver::send_RequestDepthAndGain()
{
	this->sendCommand("", "Get", "<Command Name=\"Get\" DeviceId=\"VideoDevice\" > <Parameter Name=\"Depth\"/> <Parameter Name=\"Gain\"/> <Parameter Name=\"Unknown\"/> </Command>");

	//Reply should be:
	// <Command><Result>true</Result><Message>
	// <Result success=true> <Parameter Name="Depth" /> </Result>
	// <Result success=true> <Parameter Name="Gain" /> </Result>
	// <Result success=false> <Parameter Name="Unknown" /> </Result>
	// </Message></Command>
}

void PlusReceiver::send_RequestDepth()
{
	this->sendCommand("", "Get", "<Command Name=\"Get\" DeviceId=\"VideoDevice\" > <Parameter Name=\"Depth\"/> </Command>");
}
void PlusReceiver::send_RequestGain()
{
	this->sendCommand("", "Get", "<Command Name=\"Get\" DeviceId=\"VideoDevice\" > <Parameter Name=\"Gain\"/> </Command>");
}
void PlusReceiver::send_RequestUsSectorParameters()
{
	QString command;
	command += "<Command Name=\"Get\" DeviceId=\"VideoDevice\" >";
	command += "<Parameter Name=\"Gain\"/>";
	command += "<Parameter Name=\"ProbeType\"/>";
	command += "<Parameter Name=\"StartDepth\"/>";
	command += "<Parameter Name=\"StopDepth\"/>";
	command += "</Command>";
	this->sendCommand("", "Get", command.toStdString());
}


} //namespace cxtest

