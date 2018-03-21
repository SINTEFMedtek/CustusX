/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXIGTLINKCONVERSION_H_
#define CXIGTLINKCONVERSION_H_

#include "cxOpenIGTLinkUtilitiesExport.h"

#include "igtlStringMessage.h"
#include "igtlStatusMessage.h"
#include "igtlImageMessage.h"
#include "igtlTransformMessage.h"

#include "cxIGTLinkUSStatusMessage.h"

//TODO remove
#include "cxIGTLinkImageMessage.h"

#include "cxImage.h"
#include "cxTransform3D.h"
#include "cxTool.h"

#include "cxLogger.h"

namespace cx
{

/**
 * Encode and decode OpenIGTLink messages
 *
 * \ingroup cx_resource_OpenIGTLinkUtilities
 *
 */
class cxOpenIGTLinkUtilities_EXPORT IGTLinkConversion
{
public:
    //Standard openigtlink messages
	igtl::StringMessage::Pointer encode(QString msg);
    QString decode(igtl::StringMessage::Pointer msg);
    QString decode(igtl::StatusMessage::Pointer msg);
    ImagePtr decode(igtl::ImageMessage::Pointer msg);
	Transform3D decode(igtl::TransformMessage::Pointer msg);

	/**
      * Encode the input ProbeDefinition into an IGTLink message. */
	IGTLinkUSStatusMessage::Pointer encode(ProbeDefinitionPtr);
	/**
	  * Decode the input probe and image messages to create a
	  * ProbeDefinition object based in the input base.
	  *
	  * Each message contains part of the data, the parts that
	  * are missing are simply not filled in (i.e. keep the values
	  * already present in base.)
	  *
      * Some or all of the input messages can be NULL. */
	ProbeDefinitionPtr decode(IGTLinkUSStatusMessage::Pointer probeMessage, igtl::ImageMessage::Pointer imageMsg, ProbeDefinitionPtr base);

private:
    QString convertIGTLinkStatusCodes(const int code);
};

} //namespace cx

#endif /* CXIGTLINKCONVERSION_H_ */
