// This file is part of SSC,
// a C++ Library supporting Image Guided Therapy Applications.
//
// Copyright (C) 2008- SINTEF Medical Technology
// Copyright (C) 2008- Sonowand AS
//
// SSC is owned by SINTEF Medical Technology and Sonowand AS,
// hereafter named the owners. Each particular piece of code
// is owned by the part that added it to the library.
// SSC source code and binaries can only be used by the owners
// and those with explicit permission from the owners.
// SSC shall not be distributed to anyone else.
//
// SSC is distributed WITHOUT ANY WARRANTY; without even
// the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE.
//
// See sscLicense.txt for more information.


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

#include "vtkPolyDataSource.h"
#include "vtkPolyData.h"
#include "vtkSmartPointer.h"
typedef vtkSmartPointer<vtkPolyData> vtkPolyDataPtr;

/**\brief Source for an Ultrasound sector.
 * The output vtkPolyData contains a polygon plus texture coordinates.
 * The output is given in space u: a xy vtk image space with origin in the lower-left corner,
 *
 * The class is an adapter: The polydata is set with setProbeSector, and
 * its contents are copied into the output during Execute. No other (real)
 * work is done.
 *
 * Adapted from same-named class in CustusX1
 *
 * \ingroup sscProxy
 */
class UltrasoundSectorSource : public vtkPolyDataSource 
{
public:
	static UltrasoundSectorSource *New();
	vtkTypeRevisionMacro(UltrasoundSectorSource,vtkPolyDataSource);
	void PrintSelf(ostream& os, vtkIndent indent);

	void setProbeSector(vtkPolyDataPtr sector);

protected:
	UltrasoundSectorSource();
	~UltrasoundSectorSource() {};

	void Execute();

private:
	vtkPolyDataPtr mSector; ///< polydata representation of the sector provided externally. Used as basis during Execute().

	UltrasoundSectorSource(const UltrasoundSectorSource&);  // Not implemented.
	void operator=(const UltrasoundSectorSource&);  // Not implemented.
};


#endif
