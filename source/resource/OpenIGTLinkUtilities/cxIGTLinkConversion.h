// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.

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
 *
 * \ingroup cxResourceOpenIGTLinkUtilities
 *
 * \date  Feb 19, 2013
 * \author olevs
 * \author christiana
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
	/** Filter that converts to RGB format based on a format string
	  * of the form "RGBA" or any other ordering of these four letters,
	  * the letters define the ordering of channels in the input.
	  */
	vtkImageDataPtr createFilterFormat2RGB(QString format, vtkImageDataPtr input);
	/** Filter that converts from a XYZW-format to RGB.
	  * The input indexes are the indexes or red/green/blue in the input.
	  * The alpha channel is discarded.
	  */
	vtkImageDataPtr createFilterAny2RGB(int R, int G, int B, vtkImageDataPtr input);
};

} //namespace cx

#endif /* CXIGTLINKCONVERSION_H_ */
