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

bool TurnOffDepthPeeling(
		vtkSmartPointer<vtkRenderWindow> renderWindow,
		vtkSmartPointer<vtkRenderer> renderer);///< Turn off depth peeling

bool IsDepthPeelingSupported(vtkSmartPointer<vtkRenderWindow> renderWindow,
		vtkSmartPointer<vtkRenderer> renderer,
		bool doItOffScreen);

#endif /* CXDEPTHPEELING_H_ */
