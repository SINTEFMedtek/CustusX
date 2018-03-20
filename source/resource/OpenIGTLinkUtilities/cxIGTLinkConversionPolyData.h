/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
#include "cxOpenIGTLinkUtilitiesExport.h"


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
class cxOpenIGTLinkUtilities_EXPORT IGTLinkConversionPolyData
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
