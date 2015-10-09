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
#include "vtkCamera.h"

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
}

void CXVBcameraPath::cameraRawPointsSlot(MeshPtr mesh)
{
	// Check correct meshobject (or limit in selector) Read vtkPolyData, process spline, change layout ?
	// Lage Polyline for visualisering ?

	vtkPolyDataPtr	polyDataInput = mesh->getVtkPolyData();
	vtkPoints		*vtkpoints = polyDataInput->GetPoints();
	// Get datamatrix
	Transform3D r_M_d = mesh->get_rMd();

	mNumberOfInputPoints = polyDataInput->GetNumberOfPoints();
//	std::cout << "cxVBcameraPath::cameraRawPointsSlot() - Number of Points : "
//			  << mNumberOfInputPoints << std::endl;
//	std::cout << "cxVBcameraPath, Route to target - rMd matrix : " << r_M_d << std::endl;

	mNumberOfControlPoints = mNumberOfInputPoints / 10;
	std::cout << "NumberOfControlPoints : " << mNumberOfControlPoints << std::endl;

	mSplineX = vtkSmartPointer<vtkCardinalSpline>::New();
	mSplineY = vtkSmartPointer<vtkCardinalSpline>::New();
	mSplineZ = vtkSmartPointer<vtkCardinalSpline>::New();

	// Setting the spline curve points
	double p[3];
	int indexCntrl;
	int indexPoints;

	for(int i=0;i<=mNumberOfControlPoints;i++)
	{
		int indexP = (i*mNumberOfInputPoints-1)/mNumberOfControlPoints;
		std::cout << "Adding index : " << i << " , " << indexP << std::endl;
		vtkpoints->GetPoint(indexP,p);
		mSplineX->AddPoint(i,p[0]);
		mSplineY->AddPoint(i,p[1]);
		mSplineZ->AddPoint(i,p[2]);
	}

}

void CXVBcameraPath::cameraPathPositionSlot(int pos)
{
//	std::cout << "CXVBcameraPath::cameraPathPositionSlot , pos " << pos << std::endl;
//	std::cout << "Check spline -------- " << std::endl;
//	std::cout << "Spline point : " << mSplineX->Evaluate(pos) << " , " << mSplineY->Evaluate(pos)
//			  << " , " << mSplineZ->Evaluate(pos) << std::endl;
//	std::cout << "Manual Tool : " << std::endl;
//	std::cout << mManualTool->get_prMt() << std::endl;
//	std::cout << "Patient registration : " << std::endl;

//	Transform3D rMpr = mPatientModelService->get_rMpr();
//	Transform3D rMt = rMpr * mManualTool->get_prMt();
//	std::cout << rMt << std::endl;
	double splineParameter = pos*mNumberOfControlPoints / 100.0;
//	std::cout << "CXVBcameraPath::cameraPathPositionSlot , pos : " << pos
//			  << ", spline parameter : " << splineParameter << std::endl;

	double pos_r[3], focus_r[3];

	pos_r[0] = mSplineX->Evaluate(splineParameter);
	pos_r[1] = mSplineY->Evaluate(splineParameter);
	pos_r[2] = mSplineZ->Evaluate(splineParameter);
	focus_r[0] = mSplineX->Evaluate(splineParameter+0.5);
	focus_r[1] = mSplineY->Evaluate(splineParameter+0.5);
	focus_r[2] = mSplineZ->Evaluate(splineParameter+0.5);


	// If camera position approaches end point on spline, keep the last position
	// and focus to make sure it wont be set to invalid values
	if(splineParameter<((double)(mNumberOfControlPoints)-0.5)) {
		mLastCameraPos_r = Vector3D(pos_r[0], pos_r[1], pos_r[2]);
		mLastCameraFocus_r = Vector3D(focus_r[0], focus_r[1], focus_r[2]);
	}

//	std::cout << "Position : " << pos_r[0] << ", " << pos_r[1] << ", " << pos_r[2] << std::endl;
//	std::cout << "Focus: " << focus_r[0] << ", " << focus_r[1] << ", " << focus_r[2] << std::endl;

	this->updateManualToolPosition();

}

void CXVBcameraPath::updateManualToolPosition()
{
//	Vector3D xVector = Vector3D(0,1,0);
//	Vector3D yVector = Vector3D(1,0,0);
//	Vector3D zVector = Vector3D(0,0,-1);
	// New View direction
	Vector3D viewDirection_r = (mLastCameraFocus_r - mLastCameraPos_r).normalized();
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
	Transform3D prMt = rMpr.inv() * rMt * rotateX * rotateZ;

	mManualTool->set_prMt(prMt);

}

void CXVBcameraPath::cameraViewAngleSlot(int angle)
{
//	std::cout << "CXVBcameraPath::cameraViewAngleSlot : " << angle << std::endl;
	mLastCameraViewAngle = static_cast<double>(angle) * (M_PI / 180);
	this->updateManualToolPosition();
	mViewService->get3DView()->setModified();
}

void CXVBcameraPath::cameraRotateAngleSlot(int angle)
{
	std::cout << "CXVBcameraPath::cameraRotateAngleSlot : " << angle << std::endl;
	mLastCameraRotAngle = static_cast<double>(angle) * (M_PI / 180);
	this->updateManualToolPosition();
	mViewService->get3DView()->setModified();
}

} /* namespace cx */
