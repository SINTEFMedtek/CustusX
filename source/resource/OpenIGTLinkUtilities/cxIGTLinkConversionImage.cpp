/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

/*==========================================================================

  Portions (c) Copyright 2008-2009 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: http://svn.slicer.org/Slicer3/trunk/Modules/OpenIGTLinkIF/vtkIGTLToMRMLImage.h $
  Date:      $Date: 2010-11-23 00:58:13 -0500 (Tue, 23 Nov 2010) $
  Version:   $Revision: 15552 $

==========================================================================*/
#include "cxIGTLinkConversionImage.h"
#include "vtkImageData.h"

#include <igtl_util.h>
#include "cxLogger.h"
#include "cxRegistrationTransform.h"
#include "cxIGTLinkConversionBase.h"


namespace cx
{

igtl::ImageMessage::Pointer IGTLinkConversionImage::encode(ImagePtr image, PATIENT_COORDINATE_SYSTEM externalSpace)
{
	igtl::ImageMessage::Pointer retval = igtl::ImageMessage::New();

	retval->SetDeviceName(cstring_cast(image->getName()));
	IGTLinkConversionBase().encode_timestamp(image->getAcquisitionTime(), retval);
	this->encode_vtkImageData(image->getBaseVtkImageData(), retval);
	this->encode_rMd(image, retval, externalSpace);

	return retval;
}

ImagePtr IGTLinkConversionImage::decode(igtl::ImageMessage *msg)
{
	vtkImageDataPtr vtkImage = this->decode_vtkImageData(msg);
	QDateTime timestamp = IGTLinkConversionBase().decode_timestamp(msg);
	QString deviceName = msg->GetDeviceName();

	ImagePtr retval(new Image(deviceName, vtkImage));
	retval->setAcquisitionTime(timestamp);
	this->decode_rMd(msg, retval);

	return retval;
}



namespace { // unnamed namespace

//---------------------------------------------------------------------------
// Stream copy + byte swap
//---------------------------------------------------------------------------
int swapCopy16(igtlUint16 * dst, igtlUint16 * src, int n)
{
	igtlUint16 * esrc = src + n;
	while (src < esrc)
	{
		*dst = BYTE_SWAP_INT16(*src);
		dst ++;
		src ++;
	}
	return 1;
}

int swapCopy32(igtlUint32 * dst, igtlUint32 * src, int n)
{
	igtlUint32 * esrc = src + n;
	while (src < esrc)
	{
		*dst = BYTE_SWAP_INT32(*src);
		dst ++;
		src ++;
	}
	return 1;
}

int swapCopy64(igtlUint64 * dst, igtlUint64 * src, int n)
{
	igtlUint64 * esrc = src + n;
	while (src < esrc)
	{
		*dst = BYTE_SWAP_INT64(*src);
		dst ++;
		src ++;
	}
	return 1;
}
} // unnamed namespace

vtkImageDataPtr IGTLinkConversionImage::decode_vtkImageData(igtl::ImageMessage *imgMsg)
{
	// NOTE: This method is mostly a copy-paste from Slicer.
	// MRML and coordinate stuff are removed.
	// Avoid refactoring the internals, as it is important to keep the code similar to the origin.

	// Retrieve the image data
	int   size[3];          // image dimension
	float spacing[3];       // spacing (mm/pixel)
	int   svsize[3];        // sub-volume size
	int   svoffset[3];      // sub-volume offset
	int   scalarType;       // VTK scalar type
	int   numComponents;    // number of scalar components
	int   endian;

	scalarType = IGTLToVTKScalarType( imgMsg->GetScalarType() );
	endian = imgMsg->GetEndian();
	imgMsg->GetDimensions(size);
	imgMsg->GetSpacing(spacing);
	numComponents = imgMsg->GetNumComponents();
	imgMsg->GetSubVolume(svsize, svoffset);
	//	imgMsg->GetMatrix(matrix);

	// check if the IGTL data fits to the current MRML node
	int sizeInNode[3]={0,0,0};
	int scalarTypeInNode=VTK_VOID;
	int numComponentsInNode=0;
	// Get vtk image from MRML node
	vtkSmartPointer<vtkImageData> imageData = vtkSmartPointer<vtkImageData>::New();
	imageData->SetDimensions(size[0], size[1], size[2]);
	imageData->SetExtent(0, size[0]-1, 0, size[1]-1, 0, size[2]-1);
	imageData->SetOrigin(0.0, 0.0, 0.0);
//	imageData->SetSpacing(1.0, 1.0, 1.0); // Slicer inserts spacing into its IKTtoRAS matrix, we dont.
	imageData->SetSpacing(spacing[0], spacing[1], spacing[2]);
	imageData->AllocateScalars(scalarType, numComponents);

	// Check scalar size
	int scalarSize = imgMsg->GetScalarSize();

	int fByteSwap = 0;
	// Check if bytes-swap is required
	if (scalarSize > 1 &&
			((igtl_is_little_endian() && endian == igtl::ImageMessage::ENDIAN_BIG) ||
			 (!igtl_is_little_endian() && endian == igtl::ImageMessage::ENDIAN_LITTLE)))
	{
		// Needs byte swap
		fByteSwap = 1;
	}

	if (imgMsg->GetImageSize() == imgMsg->GetSubVolumeImageSize())
	{
		// In case that volume size == sub-volume size,
		// image is read directly to the memory area of vtkImageData
		// for better performance.
		if (fByteSwap)
		{
			switch (scalarSize)
			{
			case 2:
				swapCopy16((igtlUint16 *)imageData->GetScalarPointer(),
						   (igtlUint16 *)imgMsg->GetScalarPointer(),
						   imgMsg->GetSubVolumeImageSize() / 2);
				break;
			case 4:
				swapCopy32((igtlUint32 *)imageData->GetScalarPointer(),
						   (igtlUint32 *)imgMsg->GetScalarPointer(),
						   imgMsg->GetSubVolumeImageSize() / 4);
				break;
			case 8:
				swapCopy64((igtlUint64 *)imageData->GetScalarPointer(),
						   (igtlUint64 *)imgMsg->GetScalarPointer(),
						   imgMsg->GetSubVolumeImageSize() / 8);
				break;
			default:
				break;
			}
		}
		else
		{
			memcpy(imageData->GetScalarPointer(),
				   imgMsg->GetScalarPointer(), imgMsg->GetSubVolumeImageSize());
		}
	}
	else
	{
		// In case of volume size != sub-volume size,
		// image is loaded into ImageReadBuffer, then copied to
		// the memory area of vtkImageData.
		char* imgPtr = (char*) imageData->GetScalarPointer();
		char* bufPtr = (char*) imgMsg->GetScalarPointer();
		int sizei = size[0];
		int sizej = size[1];
		//int sizek = size[2];
		int subsizei = svsize[0];

		int bg_i = svoffset[0];
		//int ed_i = bg_i + svsize[0];
		int bg_j = svoffset[1];
		int ed_j = bg_j + svsize[1];
		int bg_k = svoffset[2];
		int ed_k = bg_k + svsize[2];

		if (fByteSwap)
		{
			switch (scalarSize)
			{
			case 2:
				for (int k = bg_k; k < ed_k; k ++)
				{
					for (int j = bg_j; j < ed_j; j ++)
					{
						swapCopy16((igtlUint16 *)&imgPtr[(sizei*sizej*k + sizei*j + bg_i)*scalarSize],
								(igtlUint16 *)bufPtr,
								subsizei);
						bufPtr += subsizei*scalarSize;
					}
				}
				break;
			case 4:
				for (int k = bg_k; k < ed_k; k ++)
				{
					for (int j = bg_j; j < ed_j; j ++)
					{
						swapCopy32((igtlUint32 *)&imgPtr[(sizei*sizej*k + sizei*j + bg_i)*scalarSize],
								(igtlUint32 *)bufPtr,
								subsizei);
						bufPtr += subsizei*scalarSize;
					}
				}
				break;
			case 8:
				for (int k = bg_k; k < ed_k; k ++)
				{
					for (int j = bg_j; j < ed_j; j ++)
					{
						swapCopy64((igtlUint64 *)&imgPtr[(sizei*sizej*k + sizei*j + bg_i)*scalarSize],
								(igtlUint64 *)bufPtr,
								subsizei);
						bufPtr += subsizei*scalarSize;
					}
				}
				break;
			default:
				break;
			}
		}
		else
		{
			for (int k = bg_k; k < ed_k; k ++)
			{
				for (int j = bg_j; j < ed_j; j ++)
				{
					memcpy(&imgPtr[(sizei*sizej*k + sizei*j + bg_i)*scalarSize],
							bufPtr, subsizei*scalarSize);
					bufPtr += subsizei*scalarSize;
				}
			}
		}

	}

	imageData->Modified();
	return imageData;
}

void IGTLinkConversionImage::encode_vtkImageData(vtkImageDataPtr in, igtl::ImageMessage *outmsg)
{
	// NOTE: This method is mostly a copy-paste from Slicer.
	// MRML and coordinate stuff are removed.
	// Avoid refactoring the internals, as it is important to keep the code similar to the origin.

	vtkImageDataPtr imageData = in;
	int   isize[3];          // image dimension
	int   scalarType;       // scalar type
	double *spacing;       // spacing (mm/pixel)
	int   svoffset[] = {0, 0, 0};           // sub-volume offset
	int   endian;

	scalarType = imageData->GetScalarType();
	imageData->GetDimensions(isize);
	spacing = imageData->GetSpacing();
    int numComponents = imageData->GetNumberOfScalarComponents();

	// Check endianness of the machine
	endian = igtl::ImageMessage::ENDIAN_BIG;
	if (igtl_is_little_endian())
	{
		endian = igtl::ImageMessage::ENDIAN_LITTLE;
	}

	outmsg->SetDimensions(isize);
	outmsg->SetSpacing((float)spacing[0], (float)spacing[1], (float)spacing[2]);
	outmsg->SetScalarType(scalarType);
	outmsg->SetEndian(endian);
	outmsg->SetSubVolume(isize, svoffset);
    outmsg->SetNumComponents(numComponents);
    outmsg->AllocateScalars();

	memcpy(outmsg->GetScalarPointer(),
		   imageData->GetScalarPointer(),
		   outmsg->GetImageSize());
}


void IGTLinkConversionImage::decode_rMd(igtl::ImageMessage *msg, ImagePtr out)
{
	Transform3D sMigtl = this->getMatrix(msg);

	Vector3D c = out->boundingBox().center();
	Transform3D igtlMd = createTransformTranslate(-c);

	// s is the inbound reference system, i.e. LPS or RAS.
	PATIENT_COORDINATE_SYSTEM s = this->getPatientCoordinateSystem(msg->GetCoordinateSystem());
	Transform3D rMs = createTransformFromReferenceToExternal(s).inv();

	Transform3D rMd = rMs * sMigtl * igtlMd;

	out->get_rMd_History()->setRegistration(rMd);
}

int IGTLinkConversionImage::getIgtlCoordinateSystem(PATIENT_COORDINATE_SYSTEM space) const
{
	if (space==pcsRAS)
		return igtl::ImageMessage::COORDINATE_RAS;
	if (space==pcsLPS)
		return igtl::ImageMessage::COORDINATE_LPS;

	return igtl::ImageMessage::COORDINATE_RAS; // default
}

PATIENT_COORDINATE_SYSTEM IGTLinkConversionImage::getPatientCoordinateSystem(int igtlSpace) const
{
	if (igtlSpace==igtl::ImageMessage::COORDINATE_RAS)
		return pcsRAS;
	if (igtlSpace==igtl::ImageMessage::COORDINATE_LPS)
		return pcsLPS;

	return pcsRAS; // default
}

void IGTLinkConversionImage::encode_rMd(ImagePtr image, igtl::ImageMessage *outmsg, PATIENT_COORDINATE_SYSTEM externalSpace)
{
	Transform3D rMd = image->get_rMd();

	// NOTE: there seems to be a bug in Slicer3D: LPS not supported in OpenIGTLinkIF,
	// thus using LPS will fail against Slicer3D.
	// This doesn't matter much, as POLYDATA anyway must use RAS.

	// s is the outbound reference system, i.e. LPS or RAS.
	Transform3D sMr = createTransformFromReferenceToExternal(externalSpace);
	outmsg->SetCoordinateSystem(this->getIgtlCoordinateSystem(externalSpace));

//	if (coordinateSystem=="RAS")
//	{
//		// NOTE: there seems to be a bug in Slicer: LPS not supported in OpenIGTLinkIF.
//		// This doesn't matter much, as POLYDATA anyway must use RAS.
//		sMr = createTransformLPS2RAS();
//		outmsg->SetCoordinateSystem(igtl::ImageMessage::COORDINATE_RAS);
//	}
//	else // i.e. LPS
//	{
//		sMr = Transform3D::Identity();
//		outmsg->SetCoordinateSystem(igtl::ImageMessage::COORDINATE_LPS);
//	}

	// shift origin to image center
	Vector3D c = image->boundingBox().center();
	Transform3D dMigtl = createTransformTranslate(c);
	Transform3D sMigtl = sMr * rMd * dMigtl;

//	CX_LOG_CHANNEL_DEBUG("ca") << "rMd encode\n" << rMd;
//	CX_LOG_CHANNEL_DEBUG("ca") << "sMr encode\n" << sMr;
//	CX_LOG_CHANNEL_DEBUG("ca") << "sMigtl encode\n" << sMigtl;

	this->setMatrix(outmsg, sMigtl);
}

Transform3D IGTLinkConversionImage::getMatrix(igtl::ImageMessage *msg)
{
	igtl::Matrix4x4 matrix;
	msg->GetMatrix(matrix);
	return Transform3D::fromFloatArray(matrix);
}

void IGTLinkConversionImage::setMatrix(igtl::ImageMessage *msg, Transform3D matrix)
{
	igtl::Matrix4x4 m;
	for (int r = 0; r < 4; ++r)
		for (int c = 0; c < 4; ++c)
			m[r][c] = matrix(r,c);

	msg->SetMatrix(m);
}

int IGTLinkConversionImage::IGTLToVTKScalarType(int igtlType)
{
	switch (igtlType)
	{
	case igtl::ImageMessage::TYPE_INT8: return VTK_CHAR;
	case igtl::ImageMessage::TYPE_UINT8: return VTK_UNSIGNED_CHAR;
	case igtl::ImageMessage::TYPE_INT16: return VTK_SHORT;
	case igtl::ImageMessage::TYPE_UINT16: return VTK_UNSIGNED_SHORT;
	case igtl::ImageMessage::TYPE_INT32: return VTK_UNSIGNED_LONG;
	case igtl::ImageMessage::TYPE_UINT32: return VTK_UNSIGNED_LONG;
	case igtl::ImageMessage::TYPE_FLOAT32: return VTK_FLOAT;
	case igtl::ImageMessage::TYPE_FLOAT64: return VTK_DOUBLE;
	default:
		CX_LOG_CHANNEL_ERROR("igtl") << "Invalid IGTL scalar Type: "<< igtlType;
		return VTK_VOID;
	}
}


} //namespace cx


