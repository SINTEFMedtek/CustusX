/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include <iostream>
#include <QFile>
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
#include "cxLogger.h"

namespace cx {

CXVBcameraPath::CXVBcameraPath(TrackingServicePtr tracker, PatientModelServicePtr patientModel, ViewServicePtr visualization):
	mTrackingService(tracker),
	mPatientModelService(patientModel),
	mViewService(visualization),
	mLastCameraViewAngle(0),
	mLastCameraRotAngle(0),
	mAutomaticRotation(true),
	mWritePositionsToFile(false)
{
	mManualTool = mTrackingService->getManualTool();
	mSpline = vtkParametricSplinePtr::New();
	mLastStoredViewVector.Identity();

}

void CXVBcameraPath::cameraRawPointsSlot(MeshPtr mesh)
{
	if(mRoutePositions.size() > 0)
		if(mRoutePositions.size() == mCameraRotations.size())
		{
			this->generateSplineCurve(mRoutePositions);
			return;
		}

	if(!mesh)
	{
		std::cout << "cameraRawPointsSlot is empty !" << std::endl;
		return;
	}

	this->generateSplineCurve(mesh);
}

void CXVBcameraPath::generateSplineCurve(MeshPtr mesh)
{
	vtkPolyDataPtr	polyDataInput = mesh->getTransformedPolyDataCopy(mesh->get_rMd());
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

void CXVBcameraPath::generateSplineCurve(std::vector< Eigen::Vector3d > routePositions)
{
	vtkPointsPtr vtkPoints = vtkPointsPtr::New();
	for (int i = 0; i < routePositions.size(); i++)
		vtkPoints->InsertNextPoint(routePositions[i](0),routePositions[i](1),routePositions[i](2));

	// Setting the spline curve points
	// First clean up previous stored data
	mSpline->GetXSpline()->RemoveAllPoints();
	mSpline->GetYSpline()->RemoveAllPoints();
	mSpline->GetZSpline()->RemoveAllPoints();

	mSpline->SetPoints(vtkPoints);

}

void CXVBcameraPath::cameraPathPositionSlot(int positionPermillage)
{
	mPositionPercentage = positionPercentageAdjusted(positionPermillage/10.0) / 100.0;

	//Making shorter focus distance at last 20% of path, otherwise the camera might be outside of the smallest branches.
	//Longer focus makes smoother turns at the first divisions.
	double splineFocusDistance = 0.05;
	if (mPositionPercentage > 0.8)
		splineFocusDistance = 0.02;

	double pos_r[3], focus_r[3], d_r[3];
	double splineParameterArray[3];
	splineParameterArray[0] = mPositionPercentage;
	splineParameterArray[1] = mPositionPercentage;
	splineParameterArray[2] = mPositionPercentage;

	mSpline->Evaluate(splineParameterArray, pos_r, d_r);
	splineParameterArray[0] = mPositionPercentage + splineFocusDistance;
	splineParameterArray[1] = mPositionPercentage + splineFocusDistance;
	splineParameterArray[2] = mPositionPercentage + splineFocusDistance;
	mSpline->Evaluate(splineParameterArray, focus_r, d_r);

	mLastCameraPos_r = Vector3D(pos_r[0], pos_r[1], pos_r[2]);
	mLastCameraFocus_r = Vector3D(focus_r[0], focus_r[1], focus_r[2]);

	if(mAutomaticRotation)
		if(mRoutePositions.size() > 0 && mRoutePositions.size() == mCameraRotationsSmoothed.size())
		{
			int index = (int) (mPositionPercentage * (mRoutePositions.size() - 1));
			mLastCameraRotAngle = mCameraRotationsSmoothed[index];
			//CX_LOG_DEBUG() << "mLastCameraRotAngle: " << mLastCameraRotAngle*180/M_PI;
		}

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

	// Finding xVector which is as similar to Vector3D(0,1,0) as possible but still orthogonal to viewDirection_r
	Vector3D xVector = ( Vector3D(0,1,0) - viewDirection_r * dot(viewDirection_r, Vector3D(0,1,0)) ).normalized();
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

	if(mWritePositionsToFile)
		this->writePositionToFile(prMt);


	emit rotationChanged((int) (mLastCameraRotAngle * 180/M_PI));
}

void CXVBcameraPath::setWritePositionsToFile(bool write)
{
	mWritePositionsToFile = write;
	if(mWritePositionsToFile)
		mTimeSinceStartRecording.start();
}

void CXVBcameraPath::setWritePositionsFilePath(QString path)
{
	mPositionsFilePath = path;
}

void CXVBcameraPath::writePositionToFile(Transform3D prMt)
{
	QFile positionFile(mPositionsFilePath + "_positions.txt");
	if (positionFile.open(QIODevice::Append))
	{
		QTextStream stream(&positionFile);
		stream << prMt(0,0) << " " << prMt(0,1) << " " << prMt(0,2) << " " << prMt(0,3) << endl;
		stream << prMt(1,0) << " " << prMt(1,1) << " " << prMt(1,2) << " " << prMt(1,3) << endl;
		stream << prMt(2,0) << " " << prMt(2,1) << " " << prMt(2,2) << " " << prMt(2,3) << endl;
	}

	QFile timestampFile(mPositionsFilePath + "_timestamps.txt");
	if (timestampFile.open(QIODevice::Append))
	{
		QTextStream stream(&timestampFile);
		stream << mTimeSinceStartRecording.elapsed() << endl;
	}

	QFile branchingPositionFile(mPositionsFilePath + "_branching.txt");
	if (branchingPositionFile.open(QIODevice::Append))
	{
		bool branchingPoint = 0;
		int originalRouteIndex = (int) (mRoutePositions.size()-1) * (1-mPositionPercentage);
		if (std::find(mBranchingIndex.begin(), mBranchingIndex.end(), originalRouteIndex) != mBranchingIndex.end())
			branchingPoint = 1;
		QTextStream stream(&branchingPositionFile);
		stream << branchingPoint << endl;
	}
}



std::vector< double > CXVBcameraPath::smoothCameraRotations(std::vector< double > cameraRotations)
{
	//Camera rotation is calculated as an average of rotation in the current position and positions ahead.
	int numberOfElements = cameraRotations.size();
	std::vector< double > cameraRotationsSmoothed = cameraRotations;

	//Checking that a second turn/bifurcation is not included in the average
	int maxPositionsToSmooth = (int) (10 * numberOfElements/100);
	int positionsToSmooth = maxPositionsToSmooth;
	for(int i=0; i<numberOfElements; i++)
	{
		positionsToSmooth = std::min((int) (positionsToSmooth+.5*numberOfElements/100), maxPositionsToSmooth);
		bool firstTurnPassed = false;
		for(int j=i+1; j<std::min(i+positionsToSmooth, numberOfElements); j++)
			if (cameraRotations[j] != cameraRotations[j-1])
			{
				if (firstTurnPassed)
				{
					positionsToSmooth = j-i;
					break;
				}
				else
					firstTurnPassed = true;
			}

		std::vector< double > averageElements(cameraRotations.begin()+i, cameraRotations.begin()+std::min(i+positionsToSmooth,numberOfElements-1));
		if(averageElements.size() > 0)
			cameraRotationsSmoothed[i] = std::accumulate(averageElements.begin(), averageElements.end(), 0.0) / averageElements.size();

	}
	return cameraRotationsSmoothed;
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

void CXVBcameraPath::setRoutePositions(std::vector< Eigen::Vector3d > routePositions)
{
	mRoutePositions = routePositions;
}

void CXVBcameraPath::setCameraRotations(std::vector< double > cameraRotations)
{
	mCameraRotations = cameraRotations;
	mCameraRotationsSmoothed = smoothCameraRotations(mCameraRotations);
}

void CXVBcameraPath::setBranchingIndexAlongRoute(std::vector< int > branchingIndex)
{
	mBranchingIndex = branchingIndex;
}

void CXVBcameraPath::setAutomaticRotation(bool automaticRotation)
{
	mAutomaticRotation = automaticRotation;
}

double positionPercentageAdjusted(double positionPercentage)
{
	//Adjusting position to make smaller steps towards end of route
		return 2*positionPercentage / (1 + positionPercentage/100.0);
}
} /* namespace cx */
