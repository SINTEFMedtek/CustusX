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



/*=========================================================================

UltrasoundSectorSource creates a polygon matching the utrasound sector.
The geometric parameters for the ultrasound sector are fetched from the 
class UltrasoundSector. The ultrasound video image will be texture
mapped onto this polygon.
=========================================================================*/
// .NAME UltrasoundSectorSource - creates a polygon matching the utrasound sector.
// .SECTION Description
// UltrasoundSectorSource creates a polygon matching the utrasound sector.
// The geometric parameters for the ultrasound sector are fetched from the 
// class UltrasoundSector. The ultrasound video image will be texture
// mapped onto this polygon.
// .SECTION See Also
// vtkDiskSource

#ifndef __UltrasoundSectorSource_h
#define __UltrasoundSectorSource_h

#include "vtkPolyDataAlgorithm.h"
#include "vtkPolyData.h"
#include "vtkSmartPointer.h"
typedef vtkSmartPointer<vtkPolyData> vtkPolyDataPtr;

/** \brief Source for an Ultrasound sector.
 * The output vtkPolyData contains a polygon plus texture coordinates.
 * The output is given in space u: a xy vtk image space with origin in the lower-left corner,
 *
 * The class is an adapter: The polydata is set with setProbeSector, and
 * its contents are copied into the output during Execute. No other (real)
 * work is done.
 *
 * Adapted from same-named class in CustusX1
 *
 * \ingroup cx_resource_visualization
 */
class UltrasoundSectorSource : public vtkPolyDataAlgorithm
{
public:
	static UltrasoundSectorSource *New();
	vtkTypeMacro(UltrasoundSectorSource,vtkPolyDataAlgorithm);
	void PrintSelf(ostream& os, vtkIndent indent);

	void setProbeSector(vtkPolyDataPtr sector);

protected:
	UltrasoundSectorSource();
	~UltrasoundSectorSource() {};

	int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);
	int RequestInformation(vtkInformation *, vtkInformationVector **, vtkInformationVector *);

private:
	vtkPolyDataPtr mSector; ///< polydata representation of the sector provided externally. Used as basis during Execute().

	UltrasoundSectorSource(const UltrasoundSectorSource&);  // Not implemented.
	void operator=(const UltrasoundSectorSource&);  // Not implemented.
};


#endif
