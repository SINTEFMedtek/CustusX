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
#ifndef CXIGTLINKCONVERSIONIMAGE_H
#define CXIGTLINKCONVERSIONIMAGE_H

#include "igtlImageMessage.h"
#include "cxImage.h"

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
 */
class IGTLinkConversionImage
{
public:
	igtl::ImageMessage::Pointer encode(ImagePtr in);
	ImagePtr decode(igtl::ImageMessage *in);

private:
	vtkImageDataPtr decode_vtkImageData(igtl::ImageMessage* in);
	void decode_rMd(igtl::ImageMessage* msg, ImagePtr out);
//	void encode_Transform3D(Transform3D rMd, igtl::ImageMessage *outmsg);
	void encode_rMd(ImagePtr image, igtl::ImageMessage *outmsg);
	void encode_vtkImageData(vtkImageDataPtr in, igtl::ImageMessage *outmsg);

private:
	int IGTLToVTKScalarType(int igtlType);

};

} //namespace cx

#endif // CXIGTLINKCONVERSIONIMAGE_H
