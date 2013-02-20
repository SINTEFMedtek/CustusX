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
#include "cxIGTLinkImageMessage.h"
#include "cxIGTLinkUSStatusMessage.h"

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

	/**
	  * Encode the image into a IGTLink message, containing
	  * image data, uid and timstamp
	  */
	IGTLinkImageMessage::Pointer encode(ssc::ImagePtr image);
	/**
	  * Decode the IGTLink message to create an image containing
	  * image data, uid and timstamp
	  */
	ssc::ImagePtr decode(IGTLinkImageMessage::Pointer msg);

	/**
	  * Encode the input probedata into an IGTLink message.
	  */
	IGTLinkUSStatusMessage::Pointer encode(ssc::ProbeData);
	/**
	  * Decode the input probe and image messages to create a
	  * ssc::ProbeData object based in the input base.
	  *
	  * Each message contains part of the data, the parts that
	  * are missing are simply not filled in (i.e. keep the values
	  * already present in base.)
	  *
	  * Some or all of the input messages can be NULL.
	  */
	ssc::ProbeData decode(IGTLinkUSStatusMessage::Pointer probeMessage, IGTLinkImageMessage::Pointer imageMessage, ssc::ProbeData base);

private:
//	ProbePtr getValidProbe();
};

} //namespace cx

#endif /* CXIGTLINKCONVERSION_H_ */
