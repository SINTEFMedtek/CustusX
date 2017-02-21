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

typedef vtkSmartPointer<class vtkCardinalSpline> vtkCardinalSplinePtr;

namespace cx
{


CenterlineRegistration::CenterlineRegistration()
{
    mFixedPointSet = PointSetType::New();
    mMovingPointSet = PointSetType::New();
    mRegistration = RegistrationType::New();
    mTransform = TransformType::New();
    MetricType::Pointer         metric = MetricType::New();
    mOptimizer = OptimizerType::New();
    mOptimizer->SetUseCostFunctionGradient(false);
    OptimizerType::ScalesType   scales(mTransform->GetNumberOfParameters());

    unsigned long   numberOfIterations = 2000;
    double          gradientTolerance = 1e-4;
    double          valueTolerance = 1e-4;
    double          epsilonFunction = 1e-5;

    UpdateScales(true,true,true,true,true,true);
    mOptimizer->SetNumberOfIterations(numberOfIterations);
    mOptimizer->SetValueTolerance(valueTolerance);
    mOptimizer->SetGradientTolerance(gradientTolerance);
    mOptimizer->SetEpsilonFunction(epsilonFunction);

    // Initialize transform
    mTransform->SetIdentity();
    mRegistration->SetInitialTransformParameters(mTransform->GetParameters());

    // Setup framework
    mRegistration->SetMetric(metric);
    mRegistration->SetOptimizer(mOptimizer);
    mRegistration->SetTransform(mTransform);

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

    OptimizerType::ScalesType   scales(mTransform->GetNumberOfParameters());
    const double translationScale = 40.0;    // dynamic range of translations
    const double rotationScale = 0.3;    // dynamic range of rotations

    if (xRot)
        scales[0] = 1.0 / rotationScale;
    else
        scales[0] = 1e50;
    if (yRot)
        scales[1] = 1.0 / rotationScale;
    else
        scales[1] = 1e50;
    if (zRot)
        scales[2] = 1.0 / rotationScale;
    else
        scales[2] = 1e50;
    if (xTrans)
        scales[3] = 1.0 / translationScale;
    else
        scales[3] = 1e50;
    if (yTrans)
        scales[4] = 1.0 / translationScale;
    else
        scales[4] = 1e50;
    if (zTrans)
        scales[5] = 1.0 / translationScale;
    else
        scales[5] = 1e50;

    mOptimizer->SetScales(scales);
    mRegistration->SetOptimizer(mOptimizer);

    std::cout << "Update scales: "
              << mRegistration->GetOptimizer()->GetScales()
              << scales
              << std::endl;

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
            std::cout << "spline point: " << "(" << indexSpline << ") " << p[0] << " " << p[1]  << " " << p[2] << std::endl;
        }

        //Always add the last point to complete spline
        double p[3];
        centerline->GetPoint( numberOfInputPoints-1, p );
        splineX->AddPoint( numberOfOutputPoints-1, p[0] );
        splineY->AddPoint( numberOfOutputPoints-1, p[1] );
        splineZ->AddPoint( numberOfOutputPoints-1, p[2] );
        std::cout << "spline point: " << "(" << numberOfOutputPoints-1 << ") " << p[0] << " " << p[1]  << " " << p[2] << std::endl;

        //evaluate spline - get smoothed positions
        for(int i=0; i<numberOfOutputPoints; i++)
        {
            double splineParameter = i;
            newCenterline->InsertNextPoint(splineX->Evaluate(splineParameter),
                                           splineY->Evaluate(splineParameter),
                                           splineZ->Evaluate(splineParameter));

            double p[3];
            newCenterline->GetPoint(i,p);
            std::cout << p[0] << " " << p[1]  << " " << p[2] << std::endl;
        }
    }
    else
        return centerline;

    std::cout << "smoothPostitions - number of input points: " << numberOfInputPoints << std::endl;
    std::cout << "smoothPostitions - number of output points: " << newCenterline->GetNumberOfPoints() << std::endl;
    return newCenterline;
}

vtkPointsPtr CenterlineRegistration::processCenterline(vtkPolyDataPtr centerline, Transform3D rMd)
{
    std::cout << "rMd: " << rMd << std::endl;

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

    //sort?
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

void   CenterlineRegistration::SetFixedPoints(vtkPointsPtr vtkPoints)
{
    std::cout << "CenterlineRegistration::SetFixedPoints" << std::endl;
    mFixedPointSet->Initialize();           // Clear previous pointset
//    PointsContainerPtr  itkPoints = mFixedPointSet->GetPoints();
    PointsContainerPtr  itkPoints = PointsContainer::New();
    PointType   point;

    std::cout << "CenterlineRegistration::SetFixedPoints" << " - Number of points : "
              << vtkPoints->GetNumberOfPoints() << std::endl;

    for(vtkIdType n=0;n<vtkPoints->GetNumberOfPoints();n++)
    {
        double temp_Point[3];
        vtkPoints->GetPoint(n, temp_Point);
        point[0] = temp_Point[0];
        point[1] = temp_Point[1];
        point[2] = temp_Point[2];
        Vector3D vPoint(temp_Point[0], temp_Point[1], temp_Point[2]);
//        std::cout << vPoint << std::endl;
        itkPoints->InsertElement(n,point);
    }
    mFixedPointSet->SetPoints(itkPoints);
    std::cout << "CenterlineRegistration::SetFixedPoints" << " - Number of points : "
              << itkPoints->Size() << std::endl;
//    std::cout << itkPoints << std::endl;
}

void CenterlineRegistration::SetMovingPoints(vtkPointsPtr vtkPoints)
{
    std::cout << "CenterlineRegistration::SetMovingPoints()" << std::endl;
    mMovingPointSet->Initialize();
    PointsContainerPtr  itkPoints = PointsContainer::New();
    PointType   point;

    for(vtkIdType n=0;n<vtkPoints->GetNumberOfPoints();n++)
    {
        double temp_Point[3];
        vtkPoints->GetPoint(n, temp_Point);
        Vector3D vPoint(temp_Point[0], temp_Point[1], temp_Point[2]);
//        std::cout << vPoint << std::endl;
//        Vector3D transformedPoint = temp_transform * vPoint;
        point[0] = temp_Point[0];
        point[1] = temp_Point[1];
        point[2] = temp_Point[2];
        itkPoints->InsertElement(n,point);
    }
    mMovingPointSet->SetPoints(itkPoints);
    std::cout << "CenterlineRegistration::SetMovingPoints()" << " - Number of points : "
              << itkPoints->Size() /*<< " / " << mMovingPointSet->GetNumberOfPoints()*/ << std::endl;

//    for(vtkIdType n=0;n<vtkPoints->GetNumberOfPoints();n++)
//    {
//        double temp_Point[3];
//        vtkPoints->GetPoint(n, temp_Point);
//        Vector3D vPoint(temp_Point[0], temp_Point[1], temp_Point[2]);
//        Vector3D transformedPoint = temp_transform * vPoint;
//        point[0] = transformedPoint[0];
//        point[1] = transformedPoint[1];
//        point[2] = transformedPoint[2];
//        itkPoints->InsertElement(n,point);
//    }
}

Transform3D CenterlineRegistration::FullRegisterMoving(Transform3D init_transform)
{
    mRegistration->SetFixedPointSet(mFixedPointSet);
    mRegistration->SetMovingPointSet(mMovingPointSet);

    typedef itk::Matrix<double,3,3>     MatrixType;
    typedef TransformType::OffsetType   TranslateVector;
    MatrixType                          initMatrix;
    TranslateVector                     initTranslation;

    // Set initial rotation
    for(int i=0; i<3; i++)
        for(int j=0;j<3;j++) {
            double element = init_transform(i,j);
            initMatrix(i,j) = element;
        }

    // Set initial translation
    initTranslation[0] = init_transform(0,3);
    initTranslation[1] = init_transform(1,3);
    initTranslation[2] = init_transform(2,3);

    std::cout << "Transform3D CenterlineRegistration::FullRegisterMoving()" << std::endl;
        std::cout << "Input transform : " << std::endl;
        std::cout << init_transform << std::endl;
        std::cout << "Init translation part : " << initTranslation << std::endl;
        std::cout << "Matrix : " << std::endl;
        std::cout << initMatrix << std::endl;
    std::cout << "Is matrix orthogonal : " << mTransform->MatrixIsOrthogonal(initMatrix,1e-5) << std::endl;

    mTransform->SetMatrix(initMatrix, 1e-5);
    mTransform->SetTranslation(initTranslation);
//    mTransform->SetIdentity();

    // Test keeping the rotation approx. fixed
//    OptimizerType::ScalesType scales = mRegistration->GetOptimizer()->GetScales();
//    const double rotationScale = 1000.0;
//    scales[0] = 1.0 / rotationScale;
//    scales[1] = 1.0 / rotationScale;
//    scales[2] = 1.0 / rotationScale;
//    mRegistration->GetOptimizer()->SetScales(scales);
    // End test

    std::cout << "Full Registration, Initial parameters : " << std::endl;
    std::cout << mTransform->GetParameters() << std::endl;

    //    return (Transform3D::Identity());


    mRegistration->SetInitialTransformParameters(mTransform->GetParameters());


    try
    {
        mRegistration->Update();
        std::cout << "Optimizer stop condition: "
                  << mRegistration->GetOptimizer()->GetStopConditionDescription()
                  << std::endl;
        std::cout << "Optimizer scales: "
                  << mRegistration->GetOptimizer()->GetScales()
                  << std::endl;
    //        TransformType::MatrixType regMatrix = mRegistration->GetTransform()->GetMatrix();
//        TransformType::MatrixType regMatrix = mTransform->GetMatrix();
//        std::cout << "Registrationmatrix : " << std::endl;
//        std::cout << regMatrix << std::endl;
//        std::cout << "-------------------------------------" << std::endl;
//        std::cout << regMatrix[0][0] << " " << regMatrix[0][1] << " "
//                << regMatrix[0][2] << std::endl;

        // Rotation transform part
//        Eigen::MatrixX4d matrix;
//        for(int i=0; i<3; i++)
//            for(int j=0;j<3;j++) {
//                double element = regMatrix[i][j];
//                matrix(i,j) = element;
//            }

        // Translation transform part
//        matrix(0,3) = finalParameters[3];
//        matrix(1,3) = finalParameters[4];
//        matrix(2,3) = finalParameters[5];

//        return Transform3D(Eigen::Matrix4d::Identity());
//        return Transform3D(Eigen::Matrix4d (matrix));

    }
    catch (itk::ExceptionObject &exp)
    {
        std::cout << "AARegMethod - Exception caught ! " << std::endl;
        std::cout << exp << std::endl;
    }

    RegistrationType::ParametersType finalParameters =
            mRegistration->GetLastTransformParameters();

    std::cout << "AARegMethod - Final registration parameters : "
              << finalParameters << std::endl;

//    typedef itk::Transform<double, 3,3> SuperclassTransformType;
//    typedef itk::MatrixOffsetTransformBase<double, 3,3> BaseTransformType;
//    typedef BaseTransformType::Pointer BaseTransformTypePtr;
//    SuperclassTransformType::Pointer transform = mRegistration->GetTransform();

//    TransformType::MatrixType regMatrix = dynamic_cast<const BaseTransformType*>(transform->GetPointer())->GetMatrix();

    TransformType::MatrixType regMatrix = mTransform->GetMatrix();
//    std::cout << "Registrationmatrix : " << std::endl;
//    std::cout << regMatrix << std::endl;

    for(int i=0; i<3; i++)
        for(int j=0;j<3;j++) {
            double element = regMatrix(i,j);
            mResultTransform(i,j) = element;
        }

//    mResultTransform = Transform3D::Identity();
    mResultTransform(0,3) = finalParameters(3);
    mResultTransform(1,3) = finalParameters(4);
    mResultTransform(2,3) = finalParameters(5);

    mRegistrationUpdated = true;

    return Transform3D(mResultTransform);
}


Transform3D CenterlineRegistration::runCenterlineRegistration(vtkPolyDataPtr centerline, Transform3D rMd, TimedTransformMap trackingData_prMt, Transform3D old_rMpr)
{
    std::cout << "CenterlineRegistration::runCenterlineRegistration" << std::endl;

    vtkPointsPtr centerlinePoints = processCenterline(centerline, rMd);
    double p[3];
    centerlinePoints->GetPoint(0,p);
    std::cout << "Centerline point :" << p[0] << " " << p[1] << " " << p[2] << std::endl;

    vtkPointsPtr trackingPoints = ConvertTrackingDataToVTK(trackingData_prMt, old_rMpr);
    trackingPoints->GetPoint(0,p);
    std::cout << "Tracking point :" << p[0] << " " << p[1] << " " << p[2] << std::endl;

    SetFixedPoints( centerline->GetPoints() );
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
