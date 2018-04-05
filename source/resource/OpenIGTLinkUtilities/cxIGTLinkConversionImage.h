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
#ifndef CXIGTLINKCONVERSIONIMAGE_H
#define CXIGTLINKCONVERSIONIMAGE_H

#include "igtlImageMessage.h"
#include "cxImage.h"
#include "cxOpenIGTLinkUtilitiesExport.h"


namespace cx
{

/** Convert cx::Image <--> igtl::ImageMessage
 *
 * Based on the class vtkIGTLToMRMLImage from https://github.com/openigtlink/OpenIGTLinkIF.git
 * License text can be found at the start of this file.
 *
 * encode: create igtl messages
 * decode: read from igtl messages
 *
 * decode methods assume Unpack() has been called.
 * encode methods assume Pack() will be called.
 */
class cxOpenIGTLinkUtilities_EXPORT IGTLinkConversionImage
{
public:
	igtl::ImageMessage::Pointer encode(ImagePtr in, PATIENT_COORDINATE_SYSTEM externalSpace);
	ImagePtr decode(igtl::ImageMessage *in);

private:
	vtkImageDataPtr decode_vtkImageData(igtl::ImageMessage* in);
	void decode_rMd(igtl::ImageMessage* msg, ImagePtr out);
//	void encode_Transform3D(Transform3D rMd, igtl::ImageMessage *outmsg);
	void encode_rMd(ImagePtr image, igtl::ImageMessage *outmsg, PATIENT_COORDINATE_SYSTEM externalSpace);
	void encode_vtkImageData(vtkImageDataPtr in, igtl::ImageMessage *outmsg);

private:
	int IGTLToVTKScalarType(int igtlType);
	Transform3D getMatrix(igtl::ImageMessage *msg);
	void setMatrix(igtl::ImageMessage *msg, Transform3D matrix);
	int getIgtlCoordinateSystem(PATIENT_COORDINATE_SYSTEM space) const;
	PATIENT_COORDINATE_SYSTEM getPatientCoordinateSystem(int igtlSpace) const;
};

} //namespace cx

#endif // CXIGTLINKCONVERSIONIMAGE_H
