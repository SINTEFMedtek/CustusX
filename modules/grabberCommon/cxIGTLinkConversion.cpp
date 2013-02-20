/*
 * cxIGTLinkConversion.cpp
 *
 *  Created on: Feb 19, 2013
 *      Author: olevs
 */

#include "cxIGTLinkConversion.h"

#include "sscProbeData.h"

namespace cx
{

IGTLinkConversion::IGTLinkConversion()
{
	// TODO Auto-generated constructor stub

}

IGTLinkConversion::~IGTLinkConversion()
{
	// TODO Auto-generated destructor stub
}

}


IGTLinkImageMessage::Pointer IGTLinkConversion::encode(ssc::ImagePtr image)
{

}

ssc::ImagePtr IGTLinkConversion::decode(IGTLinkImageMessage::Pointer imgMsg)
{

}

IGTLinkUSStatusMessage::Pointer IGTLinkConversion::encode(ssc::ProbeData)
{

}

//'copied' from OpenIGTLinkRTSource::updateSonixStatus()
ssc::ProbeData IGTLinkConversion::decode(IGTLinkUSStatusMessage::Pointer msg)
{
	ssc::ProbeData retval;
	// start with getting a valid data object from the probe, in order to keep
	// existing values (such as temporal calibration).
	ProbePtr probe = this->getValidProbe();
	if (probe)
		retval = probe->getData();

	retval.setType(ssc::ProbeData::TYPE(msg->GetProbeType()));
	retval.setSector(
			msg->GetDepthStart(),
			msg->GetDepthEnd(),
			msg->GetWidth(),
			0);
	ssc::ProbeData::ProbeImageData imageData = retval.getImage();
	imageData.mOrigin_p = ssc::Vector3D(msg->GetOrigin());
	retval.setImage(imageData);
}

//Copied from OpenIGTLinkRTSource::getValidProbe()
ProbePtr IGTLinkConversion::getValidProbe()
{
	ssc::ToolPtr tool = ToolManager::getInstance()->findFirstProbe();
	if (!tool)
		return ProbePtr();
	ProbePtr probe = boost::shared_dynamic_cast<Probe>(tool->getProbe());
	if (!probe)
	{
		ssc::messageManager()->sendWarning("OpenIGTLinkRTSource::updateSonixStatus: Found no Probe");
		return ProbePtr();
	}

	return probe;
}
