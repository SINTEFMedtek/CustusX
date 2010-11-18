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
#include "sscProbeSector.h"
#include "sscTransform3D.h"

class UltrasoundSectorSource : public vtkPolyDataSource 
{
public:
  static UltrasoundSectorSource *New();
  vtkTypeRevisionMacro(UltrasoundSectorSource,vtkPolyDataSource);
  void PrintSelf(ostream& os, vtkIndent indent);

  void setProbeData(const ssc::ProbeSector& data);

protected:
  UltrasoundSectorSource();
  ~UltrasoundSectorSource() {};

  void Execute();


private:
  void updateSector();
  ssc::Transform3D get_tMu() const;
  ssc::ProbeSector mData;

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
