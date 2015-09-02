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

ImagePtr IGTLinkConversionImage::decode(igtl::ImageMessage *msg)
{
	vtkImageDataPtr vtkImage = this->decode_vtkImageData(msg);
//	Transform3D transform = this->decode_rMd(msg);
	QDateTime timestamp = IGTLinkConversionBase().decode_timestamp(msg);
	QString deviceName = msg->GetDeviceName();

	ImagePtr retval(new Image(deviceName, vtkImage));
	retval->setAcquisitionTime(timestamp);
	this->decode_rMd(msg, retval);
//	retval->get_rMd_History()->setRegistration(transform);
//	retval = this->decode(retval);

	return retval;
}

igtl::ImageMessage::Pointer IGTLinkConversionImage::encode(ImagePtr image)
{
	igtl::ImageMessage::Pointer retval = igtl::ImageMessage::New();
	IGTLinkConversionBase baseConverter;

	retval->SetDeviceName(cstring_cast(image->getUid()));
	baseConverter.encode_timestamp(image->getAcquisitionTime(), retval);
	this->encode_vtkImageData(image->getBaseVtkImageData(), retval);
	this->encode_rMd(image, retval);

	retval->Pack();
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

	return imageData;
}

void IGTLinkConversionImage::encode_vtkImageData(vtkImageDataPtr in, igtl::ImageMessage *outmsg)
{
	// NOTE: This method is mostly a copy-paste from Slicer.
	// MRML and coordinate stuff are removed.
	// Avoid refactoring the internals, as it is important to keep the code similar to the origin.

//	if (!mrmlNode)
//      {
//      return 0;
//      }

//    // If mrmlNode is Image node
//    if (event == vtkMRMLVolumeNode::ImageDataModifiedEvent && strcmp(mrmlNode->GetNodeTagName(), "Volume") == 0)
//      {
//      vtkMRMLScalarVolumeNode* volumeNode =
//        vtkMRMLScalarVolumeNode::SafeDownCast(mrmlNode);

//      if (!volumeNode)
//        {
//        return 0;
//        }

	vtkImageDataPtr imageData = in;
//      vtkImageData* imageData = volumeNode->GetImageData();
	  int   isize[3];          // image dimension
	  //int   svsize[3];        // sub-volume size
	  int   scalarType;       // scalar type
	  //double *origin;
	  double *spacing;       // spacing (mm/pixel)
	  //int   ncomp;
	  int   svoffset[] = {0, 0, 0};           // sub-volume offset
	  int   endian;

	  scalarType = imageData->GetScalarType();
	  //ncomp = imageData->GetNumberOfScalarComponents();
	  imageData->GetDimensions(isize);
	  //imageData->GetExtent(0, isize[0]-1, 0, isize[1]-1, 0, isize[2]-1);
	  //origin = imageData->GetOrigin();
	  spacing = imageData->GetSpacing();

	  // Check endianness of the machine
	  endian = igtl::ImageMessage::ENDIAN_BIG;
	  if (igtl_is_little_endian())
		{
		endian = igtl::ImageMessage::ENDIAN_LITTLE;
		}

//      if (this->OutImageMessage.IsNull())
//        {
//        this->OutImageMessage = igtl::ImageMessage::New();
//        }
	  outmsg->SetDimensions(isize);
	  outmsg->SetSpacing((float)spacing[0], (float)spacing[1], (float)spacing[2]);
	  outmsg->SetScalarType(scalarType);
	  outmsg->SetEndian(endian);
//	  outmsg->SetDeviceName(volumeNode->GetName());
	  outmsg->SetSubVolume(isize, svoffset);
	  outmsg->AllocateScalars();

	  memcpy(outmsg->GetScalarPointer(),
			 imageData->GetScalarPointer(),
			 outmsg->GetImageSize());

//	  // Transform
//	  vtkMatrix4x4* rtimgTransform = vtkMatrix4x4::New();
//	  volumeNode->GetIJKToRASMatrix(rtimgTransform);
//	  float ntx = rtimgTransform->Element[0][0] / (float)spacing[0];
//	  float nty = rtimgTransform->Element[1][0] / (float)spacing[0];
//	  float ntz = rtimgTransform->Element[2][0] / (float)spacing[0];
//	  float nsx = rtimgTransform->Element[0][1] / (float)spacing[1];
//	  float nsy = rtimgTransform->Element[1][1] / (float)spacing[1];
//	  float nsz = rtimgTransform->Element[2][1] / (float)spacing[1];
//	  float nnx = rtimgTransform->Element[0][2] / (float)spacing[2];
//	  float nny = rtimgTransform->Element[1][2] / (float)spacing[2];
//	  float nnz = rtimgTransform->Element[2][2] / (float)spacing[2];
//	  float px  = rtimgTransform->Element[0][3];
//	  float py  = rtimgTransform->Element[1][3];
//	  float pz  = rtimgTransform->Element[2][3];

//	  rtimgTransform->Delete();

	  // Shift the center
	  // NOTE: The center of the image should be shifted due to different
	  // definitions of image origin between VTK (Slicer) and OpenIGTLink;
	  // OpenIGTLink image has its origin at the center, while VTK image
	  // has one at the corner.

//	  float hfovi = (float)spacing[0] * (float)(isize[0]-1) / 2.0;
//	  float hfovj = (float)spacing[1] * (float)(isize[1]-1) / 2.0;
//	  float hfovk = (float)spacing[2] * (float)(isize[2]-1) / 2.0;

//	  float cx = ntx * hfovi + nsx * hfovj + nnx * hfovk;
//	  float cy = nty * hfovi + nsy * hfovj + nny * hfovk;
//	  float cz = ntz * hfovi + nsz * hfovj + nnz * hfovk;

//	  px = px + cx;
//	  py = py + cy;
//	  pz = pz + cz;

//	  igtl::Matrix4x4 matrix; // Image origin and orientation matrix
//	  matrix[0][0] = ntx;
//	  matrix[1][0] = nty;
//	  matrix[2][0] = ntz;
//	  matrix[0][1] = nsx;
//	  matrix[1][1] = nsy;
//	  matrix[2][1] = nsz;
//	  matrix[0][2] = nnx;
//	  matrix[1][2] = nny;
//	  matrix[2][2] = nnz;
//	  matrix[0][3] = px;
//	  matrix[1][3] = py;
//	  matrix[2][3] = pz;

//	  outmsg->SetMatrix(matrix);
//	  outmsg->Pack();

//	  *size = outmsg->GetPackSize();
//	  *igtlMsg = (void*)outmsg->GetPackPointer();

//      return 1;
//      }
//    else if (strcmp(mrmlNode->GetNodeTagName(), "IGTLQuery") == 0)   // If mrmlNode is query node
//      {
//      vtkMRMLIGTLQueryNode* qnode = vtkMRMLIGTLQueryNode::SafeDownCast(mrmlNode);
//      if (qnode)
//        {
//        if (qnode->GetQueryType() == vtkMRMLIGTLQueryNode::TYPE_GET)
//          {
//          if (this->GetImageMessage.IsNull())
//            {
//            this->GetImageMessage = igtl::GetImageMessage::New();
//            }
//          this->GetImageMessage->SetDeviceName(qnode->GetIGTLDeviceName());
//          this->GetImageMessage->Pack();
//          *size = this->GetImageMessage->GetPackSize();
//          *igtlMsg = this->GetImageMessage->GetPackPointer();
//          return 1;
//          }
//        /*
//        else if (qnode->GetQueryType() == vtkMRMLIGTLQueryNode::TYPE_START)
//          {
//          *size = 0;
//          return 0;
//          }
//        else if (qnode->GetQueryType() == vtkMRMLIGTLQueryNode::TYPE_STOP)
//          {
//          *size = 0;
//          return 0;
//          }
//        */
//        return 0;
//        }
//      }
//    else
//      {
//      return 0;
//      }

//    return 0;
}

void IGTLinkConversionImage::decode_rMd(igtl::ImageMessage *msg, ImagePtr out)
{
	igtl::Matrix4x4 matrix;
	msg->GetMatrix(matrix);
	Transform3D rMigtl = Transform3D::fromFloatArray(matrix);

	CX_LOG_CHANNEL_DEBUG("ca") << "rMigtl\n" << rMigtl;

	Vector3D c = out->boundingBox().center();
	Transform3D rMcorner = rMigtl * createTransformTranslate(-c);

	Transform3D rMlps;
	CX_LOG_CHANNEL_DEBUG("ca") << "rMcorner\n" << rMcorner;

	if (msg->GetCoordinateSystem() == igtl::ImageMessage::COORDINATE_RAS)
	{
		rMlps = rMcorner * createTransformLPS2RAS().inv();
	}
	else
	{
		// igtl::ImageMessage::COORDINATE_LPS, this is the internally supported one.
		rMlps = rMcorner;
	}

	CX_LOG_CHANNEL_DEBUG("ca") << "rMlps decode\n" << rMlps;

	out->get_rMd_History()->setRegistration(rMlps);
}

void IGTLinkConversionImage::encode_rMd(ImagePtr image, igtl::ImageMessage *outmsg)
{
	Transform3D rMlps = image->get_rMd();
	// convert to RAS
	Transform3D rMras = rMlps * createTransformLPS2RAS();
	// shift origin to image center
	Vector3D c = image->boundingBox().center();
	Transform3D rMigtl = rMras * createTransformTranslate(c);

	CX_LOG_CHANNEL_DEBUG("ca") << "rMlps encode\n" << rMlps;
	CX_LOG_CHANNEL_DEBUG("ca") << "rMras encode\n" << rMras;
	CX_LOG_CHANNEL_DEBUG("ca") << "rMigtl encode\n" << rMigtl;

	igtl::Matrix4x4 matrix;
	for (int r = 0; r < 4; ++r)
		for (int c = 0; c < 4; ++c)
			matrix[r][c] = rMigtl(r,c);

	// there seems to be a bug in Slicer: LPS not supported in OpenIGTLinkIF.
	outmsg->SetCoordinateSystem(igtl::ImageMessage::COORDINATE_RAS);
//	outmsg->SetCoordinateSystem(igtl::ImageMessage::COORDINATE_LPS);
	outmsg->SetMatrix(matrix);
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


