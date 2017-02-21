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
#ifndef CENTERLINEREGISTRATION_H_
#define CENTERLINEREGISTRATION_H_

#include <vector>
#include "cxVector3D.h"
#include "cxTransform3D.h"
#include "org_custusx_registration_method_centerline_Export.h"
#include "vtkForwardDeclarations.h"
#include <map>
#include "cxTransform3D.h"
#include <vtkSmartPointer.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkLandmarkTransform.h>

#include <itkEuler3DTransform.h>
#include <itkEuclideanDistancePointMetric.h>
#include <itkLevenbergMarquardtOptimizer.h>
#include <itkPointSetToPointSetRegistrationMethod.h>
#include <itkPointSet.h>


typedef std::vector< Eigen::Matrix4d > M4Vector;


namespace cx
{

typedef std::map<double, Transform3D> TimedTransformMap;
typedef vtkSmartPointer<vtkDoubleArray>             vtkDoubleArrayPtr;
typedef vtkSmartPointer<vtkPoints>                  vtkPointsPtr;
typedef vtkSmartPointer<vtkPolyData>                vtkPolyDataPtr;

class CenterlineRegistration
{

public:
    typedef itk::PointSet< float, 3 >                   PointSetType;
    typedef PointSetType::PointType                     PointType;
    typedef PointSetType::PointsContainer               PointsContainer;
    typedef PointSetType::PointsContainerPointer        PointsContainerPtr;
    typedef PointsContainer::Iterator                   PointsIterator;

    typedef itk::EuclideanDistancePointMetric<
                                      PointSetType,
                                      PointSetType>
                                                        MetricType;
    typedef itk::Euler3DTransform< double >             TransformType;
    typedef itk::LevenbergMarquardtOptimizer            OptimizerType;

    typedef itk::PointSetToPointSetRegistrationMethod<
                                    PointSetType,
                                    PointSetType>       RegistrationType;

    CenterlineRegistration();
   vtkPointsPtr smoothPositions(vtkPointsPtr centerline);
    void UpdateScales(bool xRot, bool yRot, bool zRot, bool xTrans, bool yTrans, bool zTrans);
    void SetFixedPoints(vtkPointsPtr points);
    void SetMovingPoints(vtkPointsPtr points);
    Transform3D FullRegisterMoving(Transform3D init_transform);
    vtkPointsPtr processCenterline(vtkPolyDataPtr centerline, Transform3D rMd);
    vtkPointsPtr ConvertTrackingDataToVTK(TimedTransformMap trackingData_prMt, Transform3D rMpr);
    Transform3D runCenterlineRegistration(vtkPolyDataPtr centerline, Transform3D rMd, TimedTransformMap trackingData_prMt, Transform3D old_rMpr );
    virtual ~CenterlineRegistration();

private:
    PointSetType::Pointer mFixedPointSet;
    PointSetType::Pointer mMovingPointSet;

    RegistrationType::Pointer mRegistration;
    TransformType::Pointer mTransform;
    Transform3D mResultTransform;
    bool mRegistrationUpdated;
    OptimizerType::Pointer mOptimizer;

};

Eigen::Matrix4d registrationAlgorithm(M4Vector Tnavigation);
vtkPointsPtr convertTovtkPoints(Eigen::MatrixXd positions);
}//namespace cx

#endif /* CENTERLINEREGISTRATION_H_ */
