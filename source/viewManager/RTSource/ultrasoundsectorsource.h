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

/**Source for an Ultrasound sector.
 * The output vtkPolyData contains a polygon plus texture coordinates.
 * The output is given in space u: a xy vtk image space with origin in the lower-left corner,
 *
 * The class is an adapter: The polydata is set with setProbeSector, and
 * its contents are copied into the output during Execute. No other (real)
 * work is done.
 *
 * Adapted from same-named class in CustusX1
 *
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

/*
 * $Log: ultrasoundsectorsource.h,v $
 * Revision 1.1  2004/10/12 06:32:18  geirat
 * File created
 *
 */
