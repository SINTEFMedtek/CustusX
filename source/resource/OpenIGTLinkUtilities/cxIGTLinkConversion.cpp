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

#include <vtkImageImport.h>
#include <vtkImageData.h>
#include <vtkImageExtractComponents.h>
#include <vtkImageAppendComponents.h>

#include "igtl_status.h"

#include "cxLog.h"
#include "cxProbeDefinition.h"
#include "cxTypeConversions.h"

typedef vtkSmartPointer<vtkImageImport> vtkImageImportPtr;

void GetIdentityMatrix(igtl::Matrix4x4& matrix)
{
  matrix[0][0] = 1.0;  matrix[1][0] = 0.0;  matrix[2][0] = 0.0; matrix[3][0] = 0.0;
  matrix[0][1] = 0.0;  matrix[1][1] = 1.0;  matrix[2][1] = 0.0; matrix[3][1] = 0.0;
  matrix[0][2] = 0.0;  matrix[1][2] = 0.0;  matrix[2][2] = 1.0; matrix[3][2] = 0.0;
  matrix[0][3] = 0.0;  matrix[1][3] = 0.0;  matrix[2][3] = 0.0; matrix[3][3] = 1.0;
}

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
    vtkImageImportPtr imageImport = vtkImageImportPtr::New();

    // Retrive the image data
    float spacing[3]; // spacing (mm/pixel)
    int svsize[3]; // sub-volume size
    int svoffset[3]; // sub-volume offset
    int scalarType; // scalar type
    int size[3]; // image dimension

    // Note: subvolumes is not supported. Implement when needed.

    scalarType = message->GetScalarType();
    message->GetDimensions(size);
    message->GetSpacing(spacing);
    message->GetSubVolume(svsize, svoffset);
    QString deviceName = message->GetDeviceName();

    imageImport->SetNumberOfScalarComponents(1);

    switch (scalarType)
    {
    case IGTLinkImageMessage::TYPE_INT8:
        CX_LOG_WARNING() << "signed char is not supported. Falling back to unsigned char.";
        imageImport->SetDataScalarTypeToUnsignedChar();
        break;
    case IGTLinkImageMessage::TYPE_UINT8:
        imageImport->SetDataScalarTypeToUnsignedChar();
        break;
    case IGTLinkImageMessage::TYPE_INT16:
        imageImport->SetDataScalarTypeToShort();
        break;
    case IGTLinkImageMessage::TYPE_UINT16:
        imageImport->SetNumberOfScalarComponents(2);
        imageImport->SetDataScalarTypeToUnsignedChar();
        break;
    case IGTLinkImageMessage::TYPE_INT32:
    case IGTLinkImageMessage::TYPE_UINT32:
        imageImport->SetNumberOfScalarComponents(4);
        imageImport->SetDataScalarTypeToUnsignedChar();
        break;
    case IGTLinkImageMessage::TYPE_FLOAT32:
        imageImport->SetDataScalarTypeToFloat();
        break;
    case IGTLinkImageMessage::TYPE_FLOAT64:
        imageImport->SetDataScalarTypeToDouble();
        break;
    default:
        CX_LOG_WARNING() << "Unknown image type. Falling back to unsigned char.";
        imageImport->SetDataScalarTypeToUnsignedChar();
    }

    imageImport->SetDataOrigin(0, 0, 0);
    imageImport->SetDataSpacing(spacing[0], spacing[1], spacing[2]);
    imageImport->SetWholeExtent(0, size[0] - 1, 0, size[1] - 1, 0, size[2] - 1);
    imageImport->SetDataExtentToWholeExtent();
    imageImport->SetImportVoidPointer(message->GetScalarPointer());

    imageImport->Modified();
    imageImport->Update();

    ImagePtr retval(new Image(deviceName, imageImport->GetOutput()));
	retval->setAcquisitionTime(this->decode_timestamp(message));
    retval = this->decode(retval);

    return retval;
}

Transform3D IGTLinkConversion::decode_image_matrix(igtl::ImageMessage::Pointer msg)
{
    igtl::Matrix4x4 matrix;
    msg->GetMatrix(matrix);
    Transform3D retval = Transform3D::fromFloatArray(matrix);

	if (msg->GetCoordinateSystem() == igtl::ImageMessage::COORDINATE_RAS)
	{
		retval = retval * createTransformLPS2RAS().inv();
	}
	// the other option, igtl::ImageMessage::COORDINATE_LPS, is the internally supported one.

    return retval;
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
	vtkImageDataPtr rawImage = image->getBaseVtkImageData();

	int   size[]     = {256, 256, 1};       // image dimension
	rawImage->GetDimensions(size);

	double spacingD[3];
	float spacingF[3];
	rawImage->GetSpacing(spacingD);
	spacingF[0] = spacingD[0];
	spacingF[1] = spacingD[1];
	spacingF[2] = spacingD[2];
	int*   svsize   = size;
	int   svoffset[] = {0, 0, 0};           // sub-volume offset
	int   scalarType = -1;

	if (rawImage->GetNumberOfScalarComponents()==4)
	{
		if (rawImage->GetScalarType()==VTK_UNSIGNED_CHAR)
		{
			scalarType = igtl::ImageMessage::TYPE_UINT32;// scalar type
		}
	}

	if (rawImage->GetNumberOfScalarComponents()==1)
	{
		if (rawImage->GetScalarType()==VTK_UNSIGNED_SHORT)
		{
			scalarType = igtl::ImageMessage::TYPE_UINT16;// scalar type
		}
		else if (rawImage->GetScalarType()==VTK_UNSIGNED_CHAR)
		{
			scalarType = igtl::ImageMessage::TYPE_UINT8;// scalar type
		}
	}

	if (scalarType==-1)
	{
		std::cerr << "Unsupported file type based on vtk " << rawImage->GetScalarTypeAsString() << std::endl;
		return IGTLinkImageMessage::Pointer();
	}

	//------------------------------------------------------------
	// Create a new IMAGE type message
	IGTLinkImageMessage::Pointer imgMsg = IGTLinkImageMessage::New();
	imgMsg->SetDimensions(size);
	imgMsg->SetSpacing(spacingF);
	imgMsg->SetScalarType(scalarType);
	imgMsg->SetDeviceName(cstring_cast(image->getUid()));
	imgMsg->SetSubVolume(svsize, svoffset);
	imgMsg->AllocateScalars();

	this->encode_timestamp(image->getAcquisitionTime(), imgMsg);

	int fsize = imgMsg->GetImageSize();
    memcpy(imgMsg->GetScalarPointer(), rawImage->GetScalarPointer(0,0,0), fsize); // not sure if we need to copy

	igtl::Matrix4x4 matrix;
    GetIdentityMatrix(matrix);
	imgMsg->SetMatrix(matrix);

	return imgMsg;
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
	QString newUid = msg->getUid();
	QString format = this->extractColorFormat(msg->getUid(), &newUid);
	vtkImageDataPtr imageRGB = this->createFilterFormat2RGB(format, msg->getBaseVtkImageData());

	// copy because the image will eventually be passed to another thread, and we cannot have the entire pipeline dragged along.
	vtkImageDataPtr copy = vtkImageDataPtr::New();
	copy->DeepCopy(imageRGB);

	ImagePtr retval(new Image(newUid, copy));
	retval->setAcquisitionTime(msg->getAcquisitionTime());
	return retval;
}

ProbeDefinitionPtr IGTLinkConversion::decode(ProbeDefinitionPtr msg)
{
	QString newUid = msg->getUid();
	QString format = this->extractColorFormat(msg->getUid(), &newUid);
	msg->setUid(newUid);

    return msg;
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

QString IGTLinkConversion::extractColorFormat(QString deviceName, QString* cleanedDeviceName)
{
	QString format = "";
	QRegExp colorFormat("\\[[A-Za-z]{1,4}\\]");
	if (colorFormat.indexIn(deviceName) > 0)
	{
		format = colorFormat.cap(0).remove("[").remove("]");
	}

	if (cleanedDeviceName)
	{
		*cleanedDeviceName = deviceName.remove(colorFormat).trimmed();
	}

	return format;
}

vtkImageDataPtr IGTLinkConversion::createFilterFormat2RGB(QString format, vtkImageDataPtr input)
{
	int R = format.indexOf('R', 0, Qt::CaseInsensitive);
	int G = format.indexOf('G', 0, Qt::CaseInsensitive);
	int B = format.indexOf('B', 0, Qt::CaseInsensitive);
	if (R<0 || G<0 || B<0 || format.size()>4)
	{
		R = 0;
		G = 1;
		B = 2;
	}

	return this->createFilterAny2RGB(R, G, B, input);
}

vtkImageDataPtr IGTLinkConversion::createFilterAny2RGB(int R, int G, int B, vtkImageDataPtr input)
{
	if (input->GetNumberOfScalarComponents() == 1)
		return input;
	if (( input->GetNumberOfScalarComponents()==3 )&&( R==0 )&&( G==1 )&&( B==2 ))
		return input;

	vtkImageAppendComponentsPtr merger = vtkImageAppendComponentsPtr::New();
	vtkImageExtractComponentsPtr splitterRGB = vtkImageExtractComponentsPtr::New();
	splitterRGB->SetInputData(input);
	splitterRGB->SetComponents(R, G, B);
	merger->AddInputConnection(splitterRGB->GetOutputPort());
	merger->Update();
	return merger->GetOutput();
}

QDateTime IGTLinkConversion::decode_timestamp(igtl::MessageBase* msg)
{
	// get timestamp from igtl second-format:
	igtl::TimeStamp::Pointer timestamp = igtl::TimeStamp::New();
	msg->GetTimeStamp(timestamp);
	double timestampMS = timestamp->GetTimeStamp() * 1000;
	return QDateTime::fromMSecsSinceEpoch(timestampMS);
}

void IGTLinkConversion::encode_timestamp(QDateTime ts, igtl::MessageBase* msg)
{
	igtl::TimeStamp::Pointer timestamp;
	timestamp = igtl::TimeStamp::New();
	double grabTime = 1.0 / 1000 * (double) ts.toMSecsSinceEpoch();
	timestamp->SetTime(grabTime);
	msg->SetTimeStamp(timestamp);
}


} // namespace cx
