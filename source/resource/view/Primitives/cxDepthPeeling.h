/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXDEPTHPEELING_H_
#define CXDEPTHPEELING_H_

#include "cxResourceVisualizationExport.h"

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

cxResourceVisualization_EXPORT vtkSmartPointer<vtkAppendPolyData> GenerateOverlappingBunchOfSpheres(int theta, int phi);

cxResourceVisualization_EXPORT bool SetupEnvironmentForDepthPeeling(vtkSmartPointer<vtkRenderWindow> renderWindow,
		vtkSmartPointer<vtkRenderer> renderer, int maxNoOfPeels,
		double occlusionRatio);

cxResourceVisualization_EXPORT bool TurnOffDepthPeeling(
		vtkSmartPointer<vtkRenderWindow> renderWindow,
		vtkSmartPointer<vtkRenderer> renderer);///< Turn off depth peeling

cxResourceVisualization_EXPORT bool IsDepthPeelingSupported(vtkSmartPointer<vtkRenderWindow> renderWindow,
		vtkSmartPointer<vtkRenderer> renderer,
		bool doItOffScreen);

#endif /* CXDEPTHPEELING_H_ */
