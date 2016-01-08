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

#ifndef CXVBCAMERAPATH_H
#define CXVBCAMERAPATH_H

#include <QObject>

#include "cxForwardDeclarations.h"
#include "cxVector3D.h"
#include "cxTransform3D.h"

typedef vtkSmartPointer<class vtkCardinalSpline> vtkCardinalSplinePtr;

namespace cx {

/**
 * Class for storing and processing the virtual
 * endoscope camera path when performing
 * virtual endoscopy
 *
 * \ingroup org_custusx_virtualbronchoscopy
 *
 * \date Aug 27, 2015
 * \author Geir Arne Tangen, SINTEF
 */

class CXVBcameraPath : public QObject
{
	Q_OBJECT

private:
	vtkCardinalSplinePtr		mSplineX;
	vtkCardinalSplinePtr		mSplineY;
	vtkCardinalSplinePtr		mSplineZ;
	TrackingServicePtr			mTrackingService;
	PatientModelServicePtr		mPatientModelService;
	ViewServicePtr				mViewService;
	GeometricRepPtr				mRep;
	ToolPtr						mManualTool;

	int							mNumberOfInputPoints;
	int							mNumberOfControlPoints;
	Vector3D					mLastCameraPos_r;
	Vector3D					mLastCameraFocus_r;
	double						mLastCameraViewAngle;
	double						mLastCameraRotAngle;

	void		updateManualToolPosition();
	void		generateSplineCurve(MeshPtr mesh);
	void		generateMeshData(MeshPtr cameraPath, Transform3D r_M_d);

public:
	CXVBcameraPath(TrackingServicePtr tracker, PatientModelServicePtr patientModel,
				   ViewServicePtr visualization);

public slots:
	void cameraRawPointsSlot(MeshPtr mesh);
	void cameraPathPositionSlot(int pos);
	void cameraViewAngleSlot(int angle);
	void cameraRotateAngleSlot(int angle);

};

} /* namespace cx */

#endif // CXVBCAMERAPATH_H
