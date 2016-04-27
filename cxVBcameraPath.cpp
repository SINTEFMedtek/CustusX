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

#include <iostream>
#include "vtkForwardDeclarations.h"
#include "vtkPolyData.h"
#include "vtkCardinalSpline.h"
#include "vtkPoints.h"
#include "vtkCellArray.h"
#include "vtkCamera.h"
#include "vtkParametricSpline.h"
#include "vtkSpline.h"

#include "cxVBcameraPath.h"
#include "cxMesh.h"
#include "cxTrackingService.h"
#include "cxPatientModelService.h"
#include "cxViewServiceProxy.h"
#include "cxView.h"

namespace cx {

CXVBcameraPath::CXVBcameraPath(TrackingServicePtr tracker, PatientModelServicePtr patientModel, ViewServicePtr visualization) :
	mTrackingService(tracker), mPatientModelService(patientModel), mViewService(visualization)
{
	mManualTool = mTrackingService->getManualTool();
    mSpline = vtkParametricSplinePtr::New();
    mLastStoredViewVector.Identity();

}

void CXVBcameraPath::cameraRawPointsSlot(MeshPtr mesh)
{

	if(!mesh)
	{
		std::cout << "cameraRawPointsSlot is empty !" << std::endl;
		return;
	}

    this->generateSplineCurve(mesh);

}

void CXVBcameraPath::generateSplineCurve(MeshPtr mesh)
{
	vtkPolyDataPtr	polyDataInput = mesh->getTransformedPolyData(mesh->get_rMd());
	vtkPoints		*vtkpoints = polyDataInput->GetPoints();

	mNumberOfInputPoints = polyDataInput->GetNumberOfPoints();

    mNumberOfControlPoints = mNumberOfInputPoints;

	// Setting the spline curve points
	// First clean up previous stored data
    mSpline->GetXSpline()->RemoveAllPoints();
    mSpline->GetYSpline()->RemoveAllPoints();
    mSpline->GetZSpline()->RemoveAllPoints();

    mSpline->SetPoints(vtkpoints);
}




void CXVBcameraPath::cameraPathPositionSlot(int pos)
{

    double splineParameter = pos / 100.0;

//	std::cout << "CXVBcameraPath::cameraPathPositionSlot , pos : " << pos
//			  << ", spline parameter : " << splineParameter << std::endl;

    double pos_r[3], focus_r[3], d_r[3];
    double splineParameterArray[3];
    splineParameterArray[0] = splineParameter;
    splineParameterArray[1] = splineParameter;
    splineParameterArray[2] = splineParameter;

    mSpline->Evaluate(splineParameterArray, pos_r, d_r);
    splineParameterArray[0] = splineParameter+0.1;
    splineParameterArray[1] = splineParameter+0.1;
    splineParameterArray[2] = splineParameter+0.1;
    mSpline->Evaluate(splineParameterArray, focus_r, d_r);

    mLastCameraPos_r = Vector3D(pos_r[0], pos_r[1], pos_r[2]);
    mLastCameraFocus_r = Vector3D(focus_r[0], focus_r[1], focus_r[2]);
    this->updateManualToolPosition();

}

void CXVBcameraPath::updateManualToolPosition()
{
    Vector3D viewDirection_r;
	// New View direction
    if(similar(mLastCameraFocus_r, mLastCameraPos_r, 0.01)) {
        viewDirection_r = mLastStoredViewVector;
    } else {
        viewDirection_r = (mLastCameraFocus_r - mLastCameraPos_r).normalized();
        mLastStoredViewVector = viewDirection_r;
    }


    Vector3D xVector = Vector3D(0,1,0);
	Vector3D yVector = cross(viewDirection_r, xVector).normalized();

	// Construct tool transform
	Transform3D rMt = Transform3D::Identity();
	rMt.matrix().col(0).head(3) = xVector;
	rMt.matrix().col(1).head(3) = yVector;
	rMt.matrix().col(2).head(3) = viewDirection_r;
	rMt.matrix().col(3).head(3) = mLastCameraPos_r;

	Transform3D rotateX = createTransformRotateX(mLastCameraViewAngle);
	Transform3D rotateZ = createTransformRotateZ(mLastCameraRotAngle);

	Transform3D rMpr = mPatientModelService->get_rMpr();
    Transform3D prMt = rMpr.inv() * rMt * rotateZ * rotateX;

	mManualTool->set_prMt(prMt);

}


void CXVBcameraPath::cameraViewAngleSlot(int angle)
{
    mLastCameraViewAngle = static_cast<double>(angle) * (M_PI / 180.0);
	this->updateManualToolPosition();
}

void CXVBcameraPath::cameraRotateAngleSlot(int angle)
{
    mLastCameraRotAngle = static_cast<double>(angle) * (M_PI / 180.0);
	this->updateManualToolPosition();
}

} /* namespace cx */
