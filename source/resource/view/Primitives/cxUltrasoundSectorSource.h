/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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

#include "cxResourceVisualizationExport.h"

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
 * \ingroup cx_resource_view
 */
class cxResourceVisualization_EXPORT UltrasoundSectorSource : public vtkPolyDataAlgorithm
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
