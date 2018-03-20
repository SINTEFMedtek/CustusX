/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#include "cxIGTLinkConversionSonixCXLegacy.h"

#include <vtkImageImport.h>
#include <vtkImageData.h>
#include <vtkImageExtractComponents.h>
#include <vtkImageAppendComponents.h>

#include "igtl_status.h"

#include "cxLog.h"
#include "cxProbeDefinition.h"
#include "cxTypeConversions.h"
#include "cxIGTLinkConversionBase.h"
#include "cxIGTLinkConversionSonixCXLegacy.h"

typedef vtkSmartPointer<vtkImageImport> vtkImageImportPtr;

namespace {
void GetIdentityMatrix(igtl::Matrix4x4& matrix)
{
  matrix[0][0] = 1.0;  matrix[1][0] = 0.0;  matrix[2][0] = 0.0; matrix[3][0] = 0.0;
  matrix[0][1] = 0.0;  matrix[1][1] = 1.0;  matrix[2][1] = 0.0; matrix[3][1] = 0.0;
  matrix[0][2] = 0.0;  matrix[1][2] = 0.0;  matrix[2][2] = 1.0; matrix[3][2] = 0.0;
  matrix[0][3] = 0.0;  matrix[1][3] = 0.0;  matrix[2][3] = 0.0; matrix[3][3] = 1.0;
}
}

namespace cx
{

ProbeDefinitionPtr IGTLinkConversionSonixCXLegacy::decode(ProbeDefinitionPtr msg)
{
	QString newUid = msg->getUid();
	QString format = this->extractColorFormat(msg->getUid(), &newUid);
	msg->setUid(newUid);

	return msg;
}

ImagePtr IGTLinkConversionSonixCXLegacy::decode(igtl::ImageMessage::Pointer message)
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
	case igtl::ImageMessage::TYPE_INT8:
		CX_LOG_WARNING() << "signed char is not supported. Falling back to unsigned char.";
		imageImport->SetDataScalarTypeToUnsignedChar();
		break;
	case igtl::ImageMessage::TYPE_UINT8:
		imageImport->SetDataScalarTypeToUnsignedChar();
		break;
	case igtl::ImageMessage::TYPE_INT16:
		imageImport->SetDataScalarTypeToShort();
		break;
	case igtl::ImageMessage::TYPE_UINT16:
		imageImport->SetNumberOfScalarComponents(2);
		imageImport->SetDataScalarTypeToUnsignedChar();
		break;
	case igtl::ImageMessage::TYPE_INT32:
	case igtl::ImageMessage::TYPE_UINT32:
		imageImport->SetNumberOfScalarComponents(4);
		imageImport->SetDataScalarTypeToUnsignedChar();
		break;
	case igtl::ImageMessage::TYPE_FLOAT32:
		imageImport->SetDataScalarTypeToFloat();
		break;
	case igtl::ImageMessage::TYPE_FLOAT64:
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
	IGTLinkConversionBase baseConverter;
	retval->setAcquisitionTime(baseConverter.decode_timestamp(message));
	retval = this->decode(retval);

	return retval;
}

igtl::ImageMessage::Pointer IGTLinkConversionSonixCXLegacy::encode(ImagePtr image)
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
		return igtl::ImageMessage::Pointer();
	}

	//------------------------------------------------------------
	// Create a new IMAGE type message
	igtl::ImageMessage::Pointer imgMsg = igtl::ImageMessage::New();
	imgMsg->SetDimensions(size);
	imgMsg->SetSpacing(spacingF);
	imgMsg->SetScalarType(scalarType);
	imgMsg->SetDeviceName(cstring_cast(image->getUid()));
	imgMsg->SetSubVolume(svsize, svoffset);
	imgMsg->AllocateScalars();

	IGTLinkConversionBase baseConverter;
	baseConverter.encode_timestamp(image->getAcquisitionTime(), imgMsg);

	int fsize = imgMsg->GetImageSize();
	memcpy(imgMsg->GetScalarPointer(), rawImage->GetScalarPointer(0,0,0), fsize); // not sure if we need to copy

	igtl::Matrix4x4 matrix;
	GetIdentityMatrix(matrix);
	imgMsg->SetMatrix(matrix);

	return imgMsg;
}


ImagePtr IGTLinkConversionSonixCXLegacy::decode(ImagePtr msg)
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

bool IGTLinkConversionSonixCXLegacy::guessIsSonixLegacyFormat(QString deviceName)
{
    QString dummy;
    QString format = this->extractColorFormat(deviceName, &dummy);
    return !format.isEmpty();
}

QString IGTLinkConversionSonixCXLegacy::extractColorFormat(QString deviceName, QString* cleanedDeviceName)
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

vtkImageDataPtr IGTLinkConversionSonixCXLegacy::createFilterFormat2RGB(QString format, vtkImageDataPtr input)
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

vtkImageDataPtr IGTLinkConversionSonixCXLegacy::createFilterAny2RGB(int R, int G, int B, vtkImageDataPtr input)
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

} //namespace cx
