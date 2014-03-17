/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkVolumePicker.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkMultiVolumePicker - ray-cast picker enhanced for multi volumes
// .SECTION Description
// vtkMultiVolumePicker handles picking inside a vtkGPUMultiVolumeRayCastMapper
//
// .SECTION See Also
// vtkPicker vtkPointPicker vtkCellPicker vtkVolumePicker

#ifndef __vtkMultiVolumePicker_h
#define __vtkMultiVolumePicker_h

#include "vtkVolumePicker.h"
#include "vtkSmartPointer.h"

typedef vtkSmartPointer<class vtkImageData> vtkImageDataPtr;
typedef vtkSmartPointer<class vtkTransform> vtkTransformPtr;
typedef vtkSmartPointer<class vtkMatrix4x4> vtkMatrix4x4Ptr;
typedef vtkSmartPointer<class vtkVolumeTextureMapper3D> vtkVolumeTextureMapper3DPtr;
typedef vtkSmartPointer<class vtkGPUVolumeRayCastMapper> vtkGPUVolumeRayCastMapperPtr;
typedef vtkSmartPointer<class vtkAbstractVolumeMapper> vtkAbstractVolumeMapperPtr;

class VTK_VOLUMERENDERING_EXPORT vtkMultiVolumePicker : public vtkVolumePicker
{
public:
  static vtkMultiVolumePicker *New();
  vtkTypeMacro(vtkMultiVolumePicker, vtkVolumePicker);
	void PrintSelf(ostream& os, vtkIndent indent);

	double* GetPickPosition();

protected:
  vtkMultiVolumePicker();
  ~vtkMultiVolumePicker();

  virtual void ResetPickInfo();

  virtual double IntersectVolumeWithLine(const double p1[3],
                                         const double p2[3],
                                         double t1, double t2,
                                         vtkProp3D *prop, 
                                         vtkAbstractVolumeMapper *mapper);


private:
  vtkMultiVolumePicker(const vtkMultiVolumePicker&);  // Not implemented.
  void operator=(const vtkMultiVolumePicker&);  // Not implemented.

	vtkTransformPtr calculate_rMdi(vtkMatrix4x4Ptr rMd0, vtkTransformPtr d0Mdi);
	void calculateNewOrigin(double* newOrigin, vtkMatrix4x4Ptr rMd0);
	vtkGPUVolumeRayCastMapperPtr generateSingleVolumeMapper(vtkImageDataPtr tempImage);
	bool similar(double a, double b, double tol = 1.0E-6); ///< check for equality with a tolerance: |b-a|<tol
	void storeFoundImage(vtkDataSet* image, vtkAbstractVolumeMapper* mapper, double* pickPosition);

	double MultivolumePickPosition[3];
	bool usedSingleVolumePicker;
};

#endif


