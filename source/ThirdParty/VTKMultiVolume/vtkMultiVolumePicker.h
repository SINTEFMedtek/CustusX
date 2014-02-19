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

class VTK_VOLUMERENDERING_EXPORT vtkMultiVolumePicker : public vtkVolumePicker
{
public:
  static vtkMultiVolumePicker *New();
  vtkTypeMacro(vtkMultiVolumePicker, vtkVolumePicker);
  void PrintSelf(ostream& os, vtkIndent indent);

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
};

#endif


