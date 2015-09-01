/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
=========================================================================*/

#ifndef CXIGTLINKCONVERSION_H_
#define CXIGTLINKCONVERSION_H_

#include "cxOpenIGTLinkUtilitiesExport.h"

#include "igtlStringMessage.h"
#include "igtlStatusMessage.h"
#include "igtlImageMessage.h"
#include "igtlTransformMessage.h"

#include "cxIGTLinkImageMessage.h"
#include "cxIGTLinkUSStatusMessage.h"

#include "cxImage.h"
#include "cxTransform3D.h"
#include "cxTool.h"

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
    Transform3D decode_image_matrix(igtl::ImageMessage::Pointer msg);
	QDateTime decode_timestamp(igtl::MessageBase* msg);
	void encode_timestamp(QDateTime ts, igtl::MessageBase* msg);
	Transform3D decode(igtl::TransformMessage::Pointer msg);

    //CustusX message formats
    /**
	  * Encode the image into a IGTLink message, containing
	  * image data, uid and timstamp
	  */
    IGTLinkImageMessage::Pointer encode(ImagePtr image);
	/**
	  * Decode the IGTLink message to create an image containing
	  * image data, uid and timstamp. The color format is also
	  * converted to RGBX
	  */
    ImagePtr decode(IGTLinkImageMessage::Pointer msg);

	/**
	  * Encode the input ProbeDefinition into an IGTLink message.
	  */
    IGTLinkUSStatusMessage::Pointer encode(ProbeDefinitionPtr);
	/**
	  * Decode the input probe and image messages to create a
	  * ProbeDefinition object based in the input base.
	  *
	  * Each message contains part of the data, the parts that
	  * are missing are simply not filled in (i.e. keep the values
	  * already present in base.)
	  *
	  * Some or all of the input messages can be NULL.
	  */
    ProbeDefinitionPtr decode(IGTLinkUSStatusMessage::Pointer probeMessage, IGTLinkImageMessage::Pointer imageMessage, ProbeDefinitionPtr base);

	/**
	  * Decode the image to standard format with standard color RGBX encoding.
	  *
	  * Find the substring [XYZW] in the msg uid, where each letter can be
	  * one of RGBAX. The letters describe the image components. Rearrange
	  * to standard RGBX format, strip format from uid,
	  * and return as new image.
	  */
    ImagePtr decode(ImagePtr msg);
    ProbeDefinitionPtr decode(ProbeDefinitionPtr msg);

private:
    QString convertIGTLinkStatusCodes(const int code);

	/** Extract the color format string from enclosing brackets inside
	  * another string, i.e find "RGBA" from "Device[RGBA]".
	  * Also return the input without format string as cleanedDeviceName.
	  */
    QString extractColorFormat(QString deviceName, QString* cleanedDeviceName);
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
