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

class UltrasoundSectorSource : public vtkPolyDataSource 
{
#define VIDEO_WIDTH 768
#define VIDEO_HEIGHT 576
//#define TEXTURESIZE 1024.0
#define TEXTURESIZE 512.0

public:
  static UltrasoundSectorSource *New();
  vtkTypeRevisionMacro(UltrasoundSectorSource,vtkPolyDataSource);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Specify inner radius of hole in disc.
  vtkSetClampMacro(InnerRadius,double,0.0,VTK_DOUBLE_MAX)
  vtkGetMacro(InnerRadius,double);

  // Description:
  // Specify outer radius of disc.
  vtkSetClampMacro(OuterRadius,double,0.0,VTK_DOUBLE_MAX)
  vtkGetMacro(OuterRadius,double);

  // Description:
  // Set the number of points in radius direction.
  vtkSetClampMacro(RadialResolution,int,1,VTK_LARGE_INTEGER)
  vtkGetMacro(RadialResolution,int);

  // Description:
  // Set the number of points in circumferential direction.
  vtkSetClampMacro(CircumferentialResolution,int,3,VTK_LARGE_INTEGER)
  vtkGetMacro(CircumferentialResolution,int);

  void setProbeData(const ssc::ProbeSector& data);

protected:
  UltrasoundSectorSource();
  ~UltrasoundSectorSource() {};

  void Execute();
  double InnerRadius;
  double OuterRadius;
  int RadialResolution;
  int CircumferentialResolution;
  ssc::ProbeSector mData;

private:
  void updateSector();
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
