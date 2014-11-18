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
#ifndef CXVIEWFOLLOWER_H
#define CXVIEWFOLLOWER_H

#include "cxResourceVisualizationExport.h"

#include <QObject>
#include "vtkForwardDeclarations.h"
#include "cxForwardDeclarations.h"
#include "cxBoundingBox3D.h"

namespace cx
{
typedef boost::shared_ptr<class ViewFollower> ViewFollowerPtr;
class DataManager;

/**
 * Ensure the tool is inside a given viewport, by moving the global center.
 *
 * \ingroup cx_resource_visualization
 * \date 2014-01-14
 * \author christiana
 */
class cxResourceVisualization_EXPORT ViewFollower : public QObject
{
	Q_OBJECT
public:
	static ViewFollowerPtr create(PatientModelServicePtr dataManager);
	void setSliceProxy(SliceProxyPtr sliceProxy);
	void setView(DoubleBoundingBox3D bb_s);

private slots:
	void ensureCenterWithinView();
private:
	explicit ViewFollower(PatientModelServicePtr dataManager);
	Vector3D findCenterShift_s();
	DoubleBoundingBox3D findStaticBox();
	Vector3D findShiftFromBoxToTool_s(DoubleBoundingBox3D BB_s, Vector3D pt_s);
	void applyShiftToCenter(Vector3D shift_s);
	Vector3D findVirtualTooltip_s();

	SliceProxyPtr mSliceProxy;
	DoubleBoundingBox3D mBB_s;
	PatientModelServicePtr mDataManager;
private:
};


} // namespace cx

#endif // CXVIEWFOLLOWER_H
