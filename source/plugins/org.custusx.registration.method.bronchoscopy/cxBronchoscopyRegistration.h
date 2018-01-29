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
#ifndef BRONCHOSCOPYREGISTRATION_H_
#define BRONCHOSCOPYREGISTRATION_H_

#include "org_custusx_registration_method_bronchoscopy_Export.h"
#include "cxBranchList.h"
#include <vector>
#include "vtkForwardDeclarations.h"

typedef std::vector< Eigen::Matrix4d > M4Vector;


namespace cx
{

typedef std::map<double, Transform3D> TimedTransformMap;
typedef boost::shared_ptr<class BranchList> BranchListPtr;

class org_custusx_registration_method_bronchoscopy_EXPORT BronchoscopyRegistration
{
	BranchListPtr mBranchListPtr;
	bool mCenterlineProcessed;

public:
	BronchoscopyRegistration();
	vtkPolyDataPtr processCenterline(vtkPolyDataPtr centerline, Transform3D rMd, int numberOfGenerations = 0);
    BranchListPtr processCenterlineImage2Image(vtkPolyDataPtr centerline, int numberOfGenerations = 0);
	Eigen::Matrix4d runBronchoscopyRegistration(TimedTransformMap trackingData_prMt, Transform3D old_rMpr, double maxDistanceForLocalRegistration);
    Eigen::Matrix4d runBronchoscopyRegistrationImage2Image(vtkPolyDataPtr centerlineFixed, vtkPolyDataPtr centerlineMoving);
	bool isCenterlineProcessed();
	virtual ~BronchoscopyRegistration();
};

M4Vector excludeClosePositions();
Eigen::Matrix4d registrationAlgorithm(BranchListPtr branches, M4Vector Tnavigation);
Eigen::Matrix4d registrationAlgorithmImage2Image(BranchListPtr branchesFixed, BranchListPtr branchesMoving);
std::vector<Eigen::MatrixXd::Index> dsearch2n(Eigen::MatrixXd pos1, Eigen::MatrixXd pos2, Eigen::MatrixXd ori1, Eigen::MatrixXd ori2);
vtkPointsPtr convertTovtkPoints(Eigen::MatrixXd positions);
Eigen::Matrix4d performLandmarkRegistration(vtkPointsPtr source, vtkPointsPtr target, bool* ok);
std::pair<Eigen::MatrixXd , Eigen::MatrixXd> RemoveInvalidData(Eigen::MatrixXd positionData, Eigen::MatrixXd orientationData);
M4Vector RemoveInvalidData(M4Vector T_vector);
org_custusx_registration_method_bronchoscopy_EXPORT Eigen::MatrixXd makeTransformedMatrix(vtkPolyDataPtr linesPolyData, Transform3D rMd = Transform3D::Identity());

}//namespace cx

#endif /* BRONCHOSCOPYREGISTRATION_H_ */
