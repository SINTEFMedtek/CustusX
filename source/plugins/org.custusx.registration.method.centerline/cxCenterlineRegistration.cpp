/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#include "cxCenterlineRegistration.h"
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkPolyDataWriter.h>
#include <vtkCellArray.h>
#include <vtkMatrix4x4.h>
#include <vtkLinearTransform.h>
#include <vtkLandmarkTransform.h>
#include "cxTransform3D.h"
#include "cxVector3D.h"
#include "cxLogger.h"
#include <boost/math/special_functions/fpclassify.hpp> // isnan
#include "vtkCardinalSpline.h"
#include <vtkKdTreePointLocator.h>
#include <algorithm>
#include <cmath>
#include <iostream>

typedef vtkSmartPointer<class vtkCardinalSpline> vtkCardinalSplinePtr;

namespace cx
{

namespace
{

typedef Eigen::Matrix<double, 6, 1> Vector6d;
typedef Eigen::Matrix<double, 6, 6> Matrix6d;

Eigen::Matrix3d rotationX(double angle)
{
	Eigen::Matrix3d retval;
	retval << 1, 0, 0,
			0, std::cos(angle), -std::sin(angle),
			0, std::sin(angle), std::cos(angle);
	return retval;
}

Eigen::Matrix3d rotationY(double angle)
{
	Eigen::Matrix3d retval;
	retval << std::cos(angle), 0, std::sin(angle),
			0, 1, 0,
			-std::sin(angle), 0, std::cos(angle);
	return retval;
}

Eigen::Matrix3d rotationZ(double angle)
{
	Eigen::Matrix3d retval;
	retval << std::cos(angle), -std::sin(angle), 0,
			std::sin(angle), std::cos(angle), 0,
			0, 0, 1;
	return retval;
}

Eigen::Matrix3d derivativeX(double angle)
{
	Eigen::Matrix3d retval;
	retval << 0, 0, 0,
			0, -std::sin(angle), -std::cos(angle),
			0, std::cos(angle), -std::sin(angle);
	return retval;
}

Eigen::Matrix3d derivativeY(double angle)
{
	Eigen::Matrix3d retval;
	retval << -std::sin(angle), 0, std::cos(angle),
			0, 0, 0,
			-std::cos(angle), 0, -std::sin(angle);
	return retval;
}

Eigen::Matrix3d derivativeZ(double angle)
{
	Eigen::Matrix3d retval;
	retval << -std::sin(angle), -std::cos(angle), 0,
			std::cos(angle), -std::sin(angle), 0,
			0, 0, 0;
	return retval;
}

/** Parameters (angleX, angleY, angleZ, tx, ty, tz) of a rotation Rz*Rx*Ry, as in itk::Euler3DTransform. */
Transform3D eulerTransform(const Vector6d& parameters)
{
	Transform3D retval = Transform3D::Identity();
	retval.linear() = rotationZ(parameters[2]) * rotationX(parameters[0]) * rotationY(parameters[1]);
	retval.translation() = parameters.tail<3>();
	return retval;
}

Vector6d eulerParameters(const Transform3D& transform)
{
	const Eigen::Matrix3d R = transform.linear();
	Vector6d retval = Vector6d::Zero();
	retval[0] = std::asin(std::max(-1.0, std::min(1.0, R(2,1))));
	const double cosX = std::cos(retval[0]);
	if (std::abs(cosX) > 0.00005)
	{
		retval[1] = std::atan2(-R(2,0) / cosX, R(2,2) / cosX);
		retval[2] = std::atan2(-R(0,1) / cosX, R(1,1) / cosX);
	}
	else
	{
		retval[1] = std::atan2(R(1,0), R(0,0));
	}
	retval.tail<3>() = transform.translation();
	return retval;
}

/** Closest fixed point for any position. */
class ClosestPoints
{
public:
	explicit ClosestPoints(const std::vector<Vector3D>& points) :
		mPoints(points)
	{
		vtkPointsPtr vtkpoints = vtkPointsPtr::New();
		for (const Vector3D& point : points)
		{
			vtkpoints->InsertNextPoint(point.data());
		}
		vtkPolyDataPtr polydata = vtkPolyDataPtr::New();
		polydata->SetPoints(vtkpoints);
		mLocator = vtkSmartPointer<vtkKdTreePointLocator>::New();
		mLocator->SetDataSet(polydata);
		mLocator->BuildLocator();
	}
	Vector3D find(const Vector3D& position) const
	{
		return mPoints[mLocator->FindClosestPoint(position.data())];
	}
private:
	std::vector<Vector3D> mPoints;
	vtkSmartPointer<vtkKdTreePointLocator> mLocator;
};

/** Levenberg-Marquardt over Euler parameters, with closest points recomputed for each pose. */
class ClosestPointRegistration
{
public:
	ClosestPointRegistration(const std::vector<Vector3D>& fixed, const std::vector<Vector3D>& moving, const bool* freeParameters) :
		mClosest(fixed),
		mMoving(moving)
	{
		std::copy(freeParameters, freeParameters + 6, mFree);
	}

	Vector6d run(const Vector6d& initialParameters) const
	{
		Vector6d parameters = initialParameters;
		double lambda = 1e-3;
		double cost = this->cost(parameters);
		for (int iteration = 0; iteration < maxIterations && lambda < 1e10; ++iteration)
		{
			const Vector6d candidate = parameters + this->step(parameters, lambda);
			const double candidateCost = this->cost(candidate);
			if (candidateCost < cost)
			{
				const bool converged = (cost - candidateCost) <= 1e-12 * (1 + cost);
				parameters = candidate;
				cost = candidateCost;
				lambda = std::max(lambda / 10, 1e-12);
				if (converged)
				{
					break;
				}
			}
			else
			{
				lambda *= 10;
			}
		}
		return parameters;
	}

private:
	static const int maxIterations = 2000;

	double cost(const Vector6d& parameters) const
	{
		const Transform3D transform = eulerTransform(parameters);
		double retval = 0;
		for (const Vector3D& point : mMoving)
		{
			const Vector3D moved = transform.coord(point);
			retval += (moved - mClosest.find(moved)).squaredNorm();
		}
		return retval;
	}

	Vector6d step(const Vector6d& parameters, double lambda) const
	{
		Matrix6d A = Matrix6d::Zero();
		Vector6d g = Vector6d::Zero();
		this->normalEquations(parameters, A, g);
		for (int i = 0; i < 6; ++i)
		{
			A(i,i) += lambda * std::max(A(i,i), 1e-9);
			if (!mFree[i])
			{
				A.row(i).setZero();
				A.col(i).setZero();
				A(i,i) = 1;
				g[i] = 0;
			}
		}
		return A.ldlt().solve(-g);
	}

	void normalEquations(const Vector6d& parameters, Matrix6d& A, Vector6d& g) const
	{
		const Eigen::Matrix3d Rx = rotationX(parameters[0]);
		const Eigen::Matrix3d Ry = rotationY(parameters[1]);
		const Eigen::Matrix3d Rz = rotationZ(parameters[2]);
		const Eigen::Matrix3d dRx = Rz * derivativeX(parameters[0]) * Ry;
		const Eigen::Matrix3d dRy = Rz * Rx * derivativeY(parameters[1]);
		const Eigen::Matrix3d dRz = derivativeZ(parameters[2]) * Rx * Ry;
		const Transform3D transform = eulerTransform(parameters);
		for (const Vector3D& point : mMoving)
		{
			const Vector3D moved = transform.coord(point);
			const Vector3D residual = moved - mClosest.find(moved);
			Eigen::Matrix<double, 3, 6> J;
			J.col(0) = dRx * point;
			J.col(1) = dRy * point;
			J.col(2) = dRz * point;
			J.rightCols<3>() = Eigen::Matrix3d::Identity();
			A += J.transpose() * J;
			g += J.transpose() * residual;
		}
	}

	ClosestPoints mClosest;
	std::vector<Vector3D> mMoving;
	bool mFree[6];
};

std::vector<Vector3D> toVector(vtkPointsPtr points)
{
	std::vector<Vector3D> retval;
	for (vtkIdType i = 0; i < points->GetNumberOfPoints(); ++i)
	{
		double p[3];
		points->GetPoint(i, p);
		retval.push_back(Vector3D(p[0], p[1], p[2]));
	}
	return retval;
}

} // namespace

CenterlineRegistration::CenterlineRegistration() :
	mResultTransform(Transform3D::Identity()),
	mRegistrationUpdated(false)
{
	UpdateScales(true,true,true,true,true,true);
}

vtkPointsPtr convertTovtkPoints(Eigen::MatrixXd positions)
{
  vtkPointsPtr retval = vtkPointsPtr::New();

  for (unsigned i=0; i<positions.cols(); ++i)
  {
    retval->InsertNextPoint(positions(0,i), positions(1,i), positions(2,i));
  }
  return retval;
}

void CenterlineRegistration::UpdateScales(bool xRot, bool yRot, bool zRot, bool xTrans, bool yTrans, bool zTrans)
{
	const bool freeParameters[6] = {xRot, yRot, zRot, xTrans, yTrans, zTrans};
	std::copy(freeParameters, freeParameters + 6, mFreeParameters);
}


vtkPointsPtr CenterlineRegistration::smoothPositions(vtkPointsPtr centerline)
{
    int numberOfInputPoints = centerline->GetNumberOfPoints();
    int controlPointFactor = 10;
    int numberOfControlPoints = numberOfInputPoints / controlPointFactor;
    if (numberOfControlPoints < 10)
        numberOfControlPoints = std::min(numberOfInputPoints,10);

    int numberOfOutputPoints = std::max(numberOfInputPoints, 100); //interpolate to at least 100 output points
    vtkPointsPtr newCenterline = vtkPointsPtr::New();;

    vtkCardinalSplinePtr splineX = vtkSmartPointer<vtkCardinalSpline>::New();
    vtkCardinalSplinePtr splineY = vtkSmartPointer<vtkCardinalSpline>::New();
    vtkCardinalSplinePtr splineZ = vtkSmartPointer<vtkCardinalSpline>::New();

    if (numberOfControlPoints >= 2)
    {
        //add control points to spline
        for(int i=0; i<numberOfControlPoints; i++)
        {
            int indexP = (i*numberOfInputPoints)/numberOfControlPoints;
            double p[3];
            centerline->GetPoint(indexP,p);
            int indexSpline = (i*numberOfOutputPoints)/numberOfControlPoints;
            splineX->AddPoint( indexSpline, p[0] );
            splineY->AddPoint( indexSpline, p[1] );
            splineZ->AddPoint( indexSpline, p[2] );
            //std::cout << "spline point: " << "(" << indexSpline << ") " << p[0] << " " << p[1]  << " " << p[2] << std::endl;
        }

        //Always add the last point to complete spline
        double p[3];
        centerline->GetPoint( numberOfInputPoints-1, p );
        splineX->AddPoint( numberOfOutputPoints-1, p[0] );
        splineY->AddPoint( numberOfOutputPoints-1, p[1] );
        splineZ->AddPoint( numberOfOutputPoints-1, p[2] );
        //std::cout << "spline point: " << "(" << numberOfOutputPoints-1 << ") " << p[0] << " " << p[1]  << " " << p[2] << std::endl;

        //evaluate spline - get smoothed positions
        for(int i=0; i<numberOfOutputPoints; i++)
        {
            double splineParameter = i;
            newCenterline->InsertNextPoint(splineX->Evaluate(splineParameter),
                                           splineY->Evaluate(splineParameter),
                                           splineZ->Evaluate(splineParameter));
        }
    }
    else
        return centerline;

    return newCenterline;
}

vtkPointsPtr CenterlineRegistration::processCenterline(vtkPolyDataPtr centerline, Transform3D rMd)
{

    vtkPointsPtr processedPositions = vtkPointsPtr::New();
	int N = centerline->GetNumberOfPoints();
	Eigen::MatrixXd CLpoints(3,N);
	for(vtkIdType i = 0; i < N; i++)
		{
        double p[3];
        centerline->GetPoint(i,p);
        Vector3D pos;
        pos(0) = p[0]; pos(1) = p[1]; pos(2) = p[2];
        pos = rMd.coord(pos);
        processedPositions->InsertNextPoint(pos[0],pos[1],pos[2]);
        }

    //sort positions?
    processedPositions = smoothPositions(processedPositions);

    return processedPositions;
}

vtkPointsPtr CenterlineRegistration::ConvertTrackingDataToVTK(TimedTransformMap trackingData_prMt, Transform3D rMpr)
{
    vtkPointsPtr loggedPositions = vtkPointsPtr::New();

    for(TimedTransformMap::iterator iter=trackingData_prMt.begin();
                iter!=trackingData_prMt.end();++iter)
            {
                Transform3D prMt = iter->second;
                Vector3D pos = prMt.matrix().block<3,1> (0, 3);
                pos = rMpr.coord(pos);
                loggedPositions->InsertNextPoint(pos[0],pos[1],pos[2]);
            }

    return loggedPositions;
}

void CenterlineRegistration::SetFixedPoints(vtkPointsPtr points)
{
	mFixedPoints = toVector(points);
}

void CenterlineRegistration::SetMovingPoints(vtkPointsPtr points)
{
	mMovingPoints = toVector(points);
}

Transform3D CenterlineRegistration::FullRegisterMoving(Transform3D init_transform)
{
	mResultTransform = init_transform;
	if (mFixedPoints.empty() || mMovingPoints.empty())
	{
		CX_LOG_WARNING() << "CenterlineRegistration: no fixed or moving points, returning the initial transform.";
	}
	else
	{
		ClosestPointRegistration registration(mFixedPoints, mMovingPoints, mFreeParameters);
		mResultTransform = eulerTransform(registration.run(eulerParameters(init_transform)));
	}
	mRegistrationUpdated = true;
	return mResultTransform;
}


Transform3D CenterlineRegistration::runCenterlineRegistration(vtkPolyDataPtr centerline, Transform3D rMd, TimedTransformMap trackingData_prMt, Transform3D old_rMpr)
{

    vtkPointsPtr centerlinePoints = processCenterline(centerline, rMd);

    SetFixedPoints( centerlinePoints );
    SetMovingPoints( ConvertTrackingDataToVTK(trackingData_prMt, old_rMpr) );

    Transform3D rMpr = FullRegisterMoving(Transform3D::Identity());

    std::cout << "rMpr : " << std::endl;
    std::cout << rMpr << std::endl;

    return rMpr;
}

CenterlineRegistration::~CenterlineRegistration()
{

}



}//namespace cx
