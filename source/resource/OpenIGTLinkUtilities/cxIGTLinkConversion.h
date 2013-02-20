/*
 * cxIGTLinkConversion.h
 *
 *  Created on: Feb 19, 2013
 *      Author: olevs
 */

#ifndef CXIGTLINKCONVERSION_H_
#define CXIGTLINKCONVERSION_H_

#include "sscImage.h"
#include "sscTool.h"

namespace cx
{

/**
 * Encode and decode IGTLink image and status messages to and from CustusX classes
 */
class IGTLinkConversion
{
public:
	IGTLinkConversion();
	virtual ~IGTLinkConversion();

	IGTLinkImageMessage::Pointer encode(ssc::ImagePtr image);
	ssc::ImagePtr decode(IGTLinkImageMessage::Pointer imgMsg);

	IGTLinkUSStatusMessage::Pointer encode(ssc::ProbeData);
	ssc::ProbeData decode(IGTLinkUSStatusMessage::Pointer msg);

private:
	ProbePtr getValidProbe();
};

} //namespace cx

#endif /* CXIGTLINKCONVERSION_H_ */
