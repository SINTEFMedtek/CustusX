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

#include "cxNavigation.h"
#include "cxImage.h"
#include "cxBoundingBox3D.h"
#include "cxPatientModelService.h"
#include "cxTrackingService.h"
#include "cxManualTool.h"
#include "cxVolumeHelpers.h"
#include "cxCameraControl.h"
#include "cxVisServices.h"
#include "cxViewService.h"

namespace cx
{

Navigation::Navigation(VisServicesPtr services, CameraControlPtr camera3D) :
	mServices(services),
	mCamera3D(camera3D)
{

}

void Navigation::centerToPosition(Vector3D p_r, QFlags<VIEW_TYPE> viewType)
{
	this->moveManualToolToPosition(p_r);

	if (viewType.testFlag(v2D))
	{
		// set center to calculated position
		mServices->patientModelService->setCenter(p_r);
	}

	if (viewType.testFlag(v3D))
	{
		if (mCamera3D)
			mCamera3D->translateByFocusTo(p_r);
	}
}

/**Place the global center to the center of the data.
 */
void Navigation::centerToData(DataPtr image)
{
	if (!image)
		return;
	Vector3D p_r = image->get_rMd().coord(image->boundingBox().center());

	this->centerToPosition(p_r);
}

/**Place the global center to the mean center of
 * a vector of data.
 */
void Navigation::centerToData(const std::vector<DataPtr>& images)
{
	Vector3D p_r = findDataCenter(images);
	this->centerToPosition(p_r);
}

void Navigation::centerToDataInActiveViewGroup(DataViewProperties properties)
{
	ViewGroupDataPtr activeGroup = mServices->visualizationService->getActiveViewGroup();
	this->centerToDataInViewGroup(activeGroup, properties);
}

void Navigation::centerToDataInViewGroup(ViewGroupDataPtr group, DataViewProperties properties)
{
	if(!group)
		return;

	std::vector<DataPtr> visibleData = group->getData(properties);
	if(visibleData.empty())
		return;

	ImagePtr activeImage = mServices->patientModelService->getActiveData<Image>();
	if(activeImage && std::count(visibleData.begin(), visibleData.end(), activeImage))
		this->centerToData(activeImage);
	else
		this->centerToData(visibleData);
}

/**Place the global center at the current position of the
 * tooltip of the active tool.
 */
void Navigation::centerToTooltip()
{
	ToolPtr tool = mServices->trackingService->getActiveTool();
	Vector3D p_pr = tool->get_prMt().coord(Vector3D(0, 0, tool->getTooltipOffset()));
	Vector3D p_r = mServices->patientModelService->get_rMpr().coord(p_pr);

	this->centerToPosition(p_r);
//	// set center to calculated position
//	mBackend->getDataManager()->setCenter(p_r);
}

/**Find the center of the images, defined as the center
 * of the smallest bounding box enclosing the images.
 */
Vector3D Navigation::findDataCenter(const std::vector<DataPtr>& data)
{
	DoubleBoundingBox3D bb_sigma = findEnclosingBoundingBox(data, Transform3D::Identity());
	return bb_sigma.center();
}

void Navigation::moveManualToolToPosition(Vector3D& p_r)
{
	// move the manual tool to the same position. (this is a side effect... do we want it?)
	ToolPtr manual = mServices->trackingService->getManualTool();
	Vector3D p_pr = mServices->patientModelService->get_rMpr().inv().coord(p_r);
	Transform3D prM0t = manual->get_prMt(); // modify old pos in order to keep orientation
	Vector3D t_pr = prM0t.coord(Vector3D(0, 0, manual->getTooltipOffset()));
	Transform3D prM1t = createTransformTranslate(p_pr - t_pr) * prM0t;

	if (!similar(prM1t, prM0t))
		manual->set_prMt(prM1t);
//  std::cout << "center manual tool" << std::endl;
}


} // namespace cx

