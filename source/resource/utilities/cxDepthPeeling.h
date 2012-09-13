/*
 * cxDepthPeeling.h
 *
 * Copied from http://www.vtk.org/Wiki/VTK/Examples/Cxx/Visualization/CorrectlyRenderingTranslucentGeometry
 *
 *  Created on: Sep 11, 2012
 *      Author: olevs
 */

#ifndef CXDEPTHPEELING_H_
#define CXDEPTHPEELING_H_

#include <vtkSphereSource.h>
#include <vtkSmartPointer.h>
#include <vtkAppendPolyData.h>
#include <vtkCamera.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkProperty.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkTimerLog.h>
#include <vtkTransform.h>
#include <vtkDepthSortPolyData.h>

vtkSmartPointer<vtkAppendPolyData> GenerateOverlappingBunchOfSpheres(int theta, int phi);

bool SetupEnvironmentForDepthPeeling(vtkSmartPointer<vtkRenderWindow> renderWindow,
		vtkSmartPointer<vtkRenderer> renderer, int maxNoOfPeels,
		double occlusionRatio);

bool IsDepthPeelingSupported(vtkSmartPointer<vtkRenderWindow> renderWindow,
		vtkSmartPointer<vtkRenderer> renderer,
		bool doItOffScreen);

#endif /* CXDEPTHPEELING_H_ */
