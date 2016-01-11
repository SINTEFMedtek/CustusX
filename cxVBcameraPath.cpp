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

#include "cxVBcameraPath.h"
#include "cxMesh.h"
#include "cxTrackingService.h"
#include "cxPatientModelService.h"
#include "cxViewServiceProxy.h"
#include "cxView.h"
#include "cxGeometricRep.h"
#include "cxRegistrationTransform.h"



namespace cx {

CXVBcameraPath::CXVBcameraPath(TrackingServicePtr tracker, PatientModelServicePtr patientModel, ViewServicePtr visualization) :
	mTrackingService(tracker), mPatientModelService(patientModel), mViewService(visualization)
{
	mManualTool = mTrackingService->getManualTool();
	mRep = GeometricRep::New();
	mSplineX = vtkSmartPointer<vtkCardinalSpline>::New();
	mSplineY = vtkSmartPointer<vtkCardinalSpline>::New();
	mSplineZ = vtkSmartPointer<vtkCardinalSpline>::New();

}

void CXVBcameraPath::cameraRawPointsSlot(MeshPtr mesh)
{
	// Check correct meshobject (or limit in selector)
	MeshPtr cameraPath;
	QString uidCameraPath;
	QString nameCameraPath;

	if(!mesh)
	{
		std::cout << "cameraRawPointsSlot is empty !" << std::endl;
		return;
	}
	// Get data transform matrix
	Transform3D r_M_d = mesh->get_rMd();

	QString uidCameraPathSubstring = QString("_cameraPath");
	QString uidCameraPathInput = mesh->getUid();
	QString nameCameraPathInput = mesh->getName();


	std::map<QString, MeshPtr> meshes = mPatientModelService->getDataOfType<Mesh>();
	for (std::map<QString, MeshPtr>::iterator iter=meshes.begin(); iter!=meshes.end(); ++iter)
		if (iter->first.contains(uidCameraPathSubstring)) {
			// Find existing camera path object and remove if found
//			std::cout << "Camera path object found .."  << std::endl;
			MeshPtr retval = iter->second;
			mPatientModelService->removeData(retval->getUid());
		}
	// Create new camera path
	if(nameCameraPathInput.contains(uidCameraPathSubstring)) {
		nameCameraPath = nameCameraPathInput;
		uidCameraPath = uidCameraPathInput;
	} else {
		nameCameraPath = nameCameraPathInput + uidCameraPathSubstring;
		uidCameraPath = uidCameraPathInput + uidCameraPathSubstring;
	}
	cameraPath = mPatientModelService->createSpecificData<Mesh>(uidCameraPath, nameCameraPath);
	this->generateSplineCurve(mesh);
	this->generateMeshData(cameraPath, r_M_d);

}

void CXVBcameraPath::generateSplineCurve(MeshPtr mesh)
{
	vtkPolyDataPtr	polyDataInput = mesh->getVtkPolyData();
	vtkPoints		*vtkpoints = polyDataInput->GetPoints();

	mNumberOfInputPoints = polyDataInput->GetNumberOfPoints();

	// Decimate the number of controlpoints to smooth the interpolated spline camera path
	mNumberOfControlPoints = mNumberOfInputPoints / 10;
//	std::cout << "NumberOfControlPoints : " << mNumberOfControlPoints << std::endl;

	// Setting the spline curve points
	double p[3];
	int indexCntrl;
	int indexPoints;

	// First clean up previous stored data
	mSplineX->RemoveAllPoints();
	mSplineY->RemoveAllPoints();
	mSplineZ->RemoveAllPoints();

	for(int i=0;i<=mNumberOfControlPoints;i++)
	{
		int indexP = (i*mNumberOfInputPoints-1)/mNumberOfControlPoints;
//		std::cout << "Adding index : " << i << " , " << indexP << std::endl;
//		std::cout << "Point : " << p[0] << ", " << p[1] << ", " << p[2] << std::endl;
		vtkpoints->GetPoint(indexP,p);
		mSplineX->AddPoint(i,p[0]);
		mSplineY->AddPoint(i,p[1]);
		mSplineZ->AddPoint(i,p[2]);
	}
}

void CXVBcameraPath::generateMeshData(MeshPtr cameraPath, Transform3D r_M_d)
{
	vtkPolyDataPtr	curvePolyData = vtkPolyDataPtr::New();
	vtkPointsPtr	curvePoints = vtkPointsPtr::New();
	vtkCellArrayPtr curveLines = vtkCellArrayPtr::New();

	double splineParameter; // [0, 8] ?

	for(int i=0;i<=79;i++)
	{
		splineParameter = i / 10.0;
		double x = mSplineX->Evaluate(splineParameter);
		double y = mSplineY->Evaluate(splineParameter);
		double z = mSplineZ->Evaluate(splineParameter);
		curvePoints->InsertPoint(i,x,y,z);

//		std::cout << "Curve Point : " << i << " , ( "
//				  << x << ", " << y << ", " << z << " )" << std::endl;

		if((i%2==0) && (i<100))	// even iterations
		{
			curveLines->InsertNextCell(2);
		}
		curveLines->InsertCellPoint(i);
	}

	curvePolyData->SetPoints(curvePoints);
	curvePolyData->SetLines(curveLines);

	cameraPath->setVtkPolyData(curvePolyData);
	cameraPath->get_rMd_History()->setRegistration(r_M_d);
	mPatientModelService->insertData(cameraPath);
	mRep->setMesh(cameraPath);
}



void CXVBcameraPath::cameraPathPositionSlot(int pos)
{

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
	mLastCameraViewAngle = static_cast<double>(angle) * (M_PI / 180);
	this->updateManualToolPosition();
	mViewService->get3DView()->setModified();
}

void CXVBcameraPath::cameraRotateAngleSlot(int angle)
{
	mLastCameraRotAngle = static_cast<double>(angle) * (M_PI / 180);
	this->updateManualToolPosition();
	mViewService->get3DView()->setModified();
}

} /* namespace cx */
