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

#include "cxIGTLinkConversion.h"

#include <vtkImageData.h>

#include "igtl_status.h"

#include "cxLog.h"
#include "cxProbeDefinition.h"
#include "cxTypeConversions.h"
#include "cxIGTLinkConversionBase.h"
#include "cxIGTLinkConversionSonixCXLegacy.h"

namespace cx
{
//--------------------------------Standard opentiglink messages---------------------------------------

igtl::StringMessage::Pointer IGTLinkConversion::encode(QString msg)
{
    igtl::StringMessage::Pointer retval;
    retval = igtl::StringMessage::New();
    retval->SetDeviceName("CustusX");
    retval->SetString(msg.toStdString().c_str());
    return retval;
}

QString IGTLinkConversion::decode(igtl::StringMessage::Pointer msg)
{
    QString devicename(msg->GetDeviceName());
    QString message(msg->GetString());
    QString retval = devicename + ": " + message;
    return retval;
}

QString IGTLinkConversion::decode(igtl::StatusMessage::Pointer msg)
{
    QString code = this->convertIGTLinkStatusCodes(msg->GetCode());
    QString retval = code;
    return retval;
}

ImagePtr IGTLinkConversion::decode(igtl::ImageMessage::Pointer message)
{
	return IGTLinkConversionSonixCXLegacy().decode(message);
}

Transform3D IGTLinkConversion::decode(igtl::TransformMessage::Pointer msg)
{
    igtl::Matrix4x4 matrix;
    msg->GetMatrix(matrix);
    Transform3D retval = Transform3D::fromFloatArray(matrix);
    return retval;
}

//--------------------------------CustusX messages---------------------------------------

IGTLinkImageMessage::Pointer IGTLinkConversion::encode(ImagePtr image)
{
	return IGTLinkConversionSonixCXLegacy().encode(image);
}

ImagePtr IGTLinkConversion::decode(IGTLinkImageMessage::Pointer message)
{
    return this->decode(igtl::ImageMessage::Pointer(message));
}

IGTLinkUSStatusMessage::Pointer IGTLinkConversion::encode(ProbeDefinitionPtr input)
{
	IGTLinkUSStatusMessage::Pointer retval = IGTLinkUSStatusMessage::New();

	retval->SetOrigin(input->getOrigin_p().data());
	// 1 = sector, 2 = linear
	retval->SetProbeType(input->getType());

	retval->SetDepthStart(input->getDepthStart());// Start of sector in mm from origin
	retval->SetDepthEnd(input->getDepthEnd());	// End of sector in mm from origin
	retval->SetWidth(input->getWidth());// Width of sector in mm for LINEAR, Width of sector in radians for SECTOR.
	retval->SetDeviceName(cstring_cast(input->getUid()));

	return retval;
}

//'copied' from OpenIGTLinkRTSource::updateSonixStatus()
ProbeDefinitionPtr IGTLinkConversion::decode(IGTLinkUSStatusMessage::Pointer probeMessage, IGTLinkImageMessage::Pointer imageMessage, ProbeDefinitionPtr base)
{
	ProbeDefinitionPtr retval;
	if (base)
		 retval = base;
	else
		retval = ProbeDefinitionPtr(new ProbeDefinition());

	if (probeMessage)
	{
		// Update the parts of the probe data that is read from the probe message.
		retval->setType(ProbeDefinition::TYPE(probeMessage->GetProbeType()));
		retval->setSector(
				probeMessage->GetDepthStart(),
				probeMessage->GetDepthEnd(),
				probeMessage->GetWidth(),
				0);
		retval->setOrigin_p(Vector3D(probeMessage->GetOrigin()));
		retval->setUid(probeMessage->GetDeviceName());
	}

	if (imageMessage)
	{
		// Update the parts of the probe data that must be read from the image.

		// Retrive the image data
		float spacing[3]; // spacing (mm/pixel)
		int size[3]; // image dimension
		imageMessage->GetDimensions(size);
		imageMessage->GetSpacing(spacing);

		retval->setSpacing(Vector3D(spacing[0], spacing[1], spacing[2]));
		retval->setSize(QSize(size[0], size[1]));
		retval->setClipRect_p(DoubleBoundingBox3D(0, retval->getSize().width(), 0, retval->getSize().height(), 0, 0));
	}

	return this->decode(retval);
}

ImagePtr IGTLinkConversion::decode(ImagePtr msg)
{
	return IGTLinkConversionSonixCXLegacy().decode(msg);
}

ProbeDefinitionPtr IGTLinkConversion::decode(ProbeDefinitionPtr msg)
{
	return IGTLinkConversionSonixCXLegacy().decode(msg);
}

QString IGTLinkConversion::convertIGTLinkStatusCodes(const int code)
{
    QString retval;
    switch(code)
    {
    case IGTL_STATUS_INVALID:
        retval = "Status invalid";
        break;
    case IGTL_STATUS_OK:
        retval = "Ok / Freeze mode on";
        break;
    case IGTL_STATUS_UNKNOWN_ERROR:
        retval = "Unknown error";
        break;
    case IGTL_STATUS_PANICK_MODE:
        retval = "Panick mode";
        break;
    case IGTL_STATUS_NOT_FOUND:
        retval = "Not found";
        break;
    case IGTL_STATUS_ACCESS_DENIED:
        retval = "Access denied";
        break;
    case IGTL_STATUS_BUSY:
        retval = "Busy";
        break;
    case IGTL_STATUS_TIME_OUT:
        retval = "Time out / Connection lost";
        break;
    case IGTL_STATUS_OVERFLOW:
        retval = "Overflow / Can't be reached";
        break;
    case IGTL_STATUS_CHECKSUM_ERROR:
        retval = "Checksum error";
        break;
    case IGTL_STATUS_CONFIG_ERROR:
        retval = "Configuration error";
        break;
    case IGTL_STATUS_RESOURCE_ERROR:
        retval = "Not enough resource (memory, storage etc)";
        break;
    case IGTL_STATUS_ILLEGAL_INSTRUCTION:
        retval = "Illegal/Unknown instruction";
        break;
    case IGTL_STATUS_NOT_READY:
        retval = "Device not ready (starting up)";
        break;
    case IGTL_STATUS_MANUAL_MODE:
        retval = "Manual mode (device does not accept commands";
        break;
    case IGTL_STATUS_DISABLED:
        retval = "Device disabled";
        break;
    case IGTL_STATUS_NOT_PRESENT:
        retval = "Device not present";
        break;
    case IGTL_STATUS_UNKNOWN_VERSION:
        retval = "Device version not known";
        break;
    case IGTL_STATUS_HARDWARE_FAILURE:
        retval = "Hardware failure";
        break;
    case IGTL_STATUS_SHUT_DOWN:
        retval = "Exiting / shut down in progress";
        break;
    default:
        retval = "Could not determine what OpenIGTLink status code means.";
    }
    return retval;
}

} // namespace cx
