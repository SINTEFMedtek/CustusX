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

  Portions (c) Copyright 2008-2014 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    vtkIGTLToMRMLPolyData.cxx

==========================================================================*/

#ifndef CXIGTLINKCONVERSIONPOLYDATA_H
#define CXIGTLINKCONVERSIONPOLYDATA_H

#include "igtlPolyDataMessage.h"
#include "cxMesh.h"

namespace cx
{

/** Convert cx::Mesh <--> igtl::PolyDataMessage
 *
 * Based on the class vtkIGTLToMRMLPolyData from https://github.com/openigtlink/OpenIGTLinkIF.git
 * License text can be found at the start of this file.
 *
 * encode: create igtl messages
 * decode: read from igtl messages
 *
 * decode methods assume Unpack() has been called.
 * encode methods assume Pack() will be called.
 */
class IGTLinkConversionPolyData
{
public:
	igtl::PolyDataMessage::Pointer encode(MeshPtr in, PATIENT_COORDINATE_SYSTEM externalSpace);
	MeshPtr decode(igtl::PolyDataMessage *in, PATIENT_COORDINATE_SYSTEM externalSpace);

private:
	vtkPolyDataPtr decode_vtkPolyData(igtl::PolyDataMessage* msg);
	void encode_vtkPolyData(vtkPolyDataPtr in, igtl::PolyDataMessage* outMsg);
	vtkPolyDataPtr decodeCoordinateSystem(vtkPolyDataPtr polyData, PATIENT_COORDINATE_SYSTEM externalSpace);
	vtkPolyDataPtr encodeCoordinateSystem(MeshPtr mesh, PATIENT_COORDINATE_SYSTEM externalSpace);

private:
	int VTKToIGTLCellArray(vtkCellArray* src, igtl::PolyDataCellArray* dest);
	int VTKToIGTLAttribute(vtkDataSetAttributes* src, int i, igtl::PolyDataAttribute* dest);

};

} //namespace cx


#endif // CXIGTLINKCONVERSIONPOLYDATA_H
