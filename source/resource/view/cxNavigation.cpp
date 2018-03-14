/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
#include "cxActiveData.h"

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
		mServices->patient()->setCenter(p_r);
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
	ViewGroupDataPtr activeGroup = mServices->view()->getActiveViewGroup();
	this->centerToDataInViewGroup(activeGroup, properties);
}

void Navigation::centerToDataInViewGroup(ViewGroupDataPtr group, DataViewProperties properties)
{
	if(!group)
		return;

	std::vector<DataPtr> visibleData = group->getData(properties);
	if(visibleData.empty())
		return;

	ActiveDataPtr active = mServices->patient()->getActiveData();
	ImagePtr activeImage = active->getActive<Image>();
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
	ToolPtr tool = mServices->tracking()->getActiveTool();
	Vector3D p_pr = tool->get_prMt().coord(Vector3D(0, 0, tool->getTooltipOffset()));
	Vector3D p_r = mServices->patient()->get_rMpr().coord(p_pr);

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
	ToolPtr manual = mServices->tracking()->getManualTool();
	Vector3D p_pr = mServices->patient()->get_rMpr().inv().coord(p_r);
	Transform3D prM0t = manual->get_prMt(); // modify old pos in order to keep orientation
	Vector3D t_pr = prM0t.coord(Vector3D(0, 0, manual->getTooltipOffset()));
	Transform3D prM1t = createTransformTranslate(p_pr - t_pr) * prM0t;

	if (!similar(prM1t, prM0t))
		manual->set_prMt(prM1t);
//  std::cout << "center manual tool" << std::endl;
}


} // namespace cx

