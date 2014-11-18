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
#include "cxVisualizationServiceBackend.h"
#include "cxData.h"
#include "cxBoundingBox3D.h"
#include "cxPatientModelService.h"
#include "cxTrackingService.h"
#include "cxManualTool.h"
#include "cxVolumeHelpers.h"
#include "cxCameraControl.h"

namespace cx
{

Navigation::Navigation(VisualizationServiceBackendPtr backend, CameraControlPtr camera3D) :
	mBackend(backend),
	mCamera3D(camera3D)
{

}

void Navigation::centerToPosition(Vector3D p_r, QFlags<VIEW_TYPE> viewType)
{
	this->moveManualToolToPosition(p_r);

	if (viewType.testFlag(v2D))
	{
		// set center to calculated position
		mBackend->getPatientService()->setCenter(p_r);
	}

	if (viewType.testFlag(v3D))
	{
		if (mCamera3D)
			mCamera3D->translateByFocusTo(p_r);
	}
}


/**Place the global center to the center of the image.
 */
void Navigation::centerToData(DataPtr image)
{
	if (!image)
		return;
	Vector3D p_r = image->get_rMd().coord(image->boundingBox().center());

	this->centerToPosition(p_r);
//	// set center to calculated position
//	mBackend->getDataManager()->setCenter(p_r);
//	this->moveManualToolToPosition(p_r);
}

/**Place the global center to the mean center of
 * all the images in a view(wrapper).
 */
void Navigation::centerToView(const std::vector<DataPtr>& images)
{
	Vector3D p_r = findViewCenter(images);
	std::cout << "center ToView: " << images.size() << " - " << p_r << std::endl;

	this->centerToPosition(p_r);
//	// set center to calculated position
//	mBackend->getDataManager()->setCenter(p_r);
//	this->moveManualToolToPosition(p_r);
}

/**Place the global center to the mean center of
 * all the loaded images.
 */
void Navigation::centerToGlobalDataCenter()
{
	if (mBackend->getPatientService()->getData().empty())
		return;

	Vector3D p_r = this->findGlobalDataCenter();

	this->centerToPosition(p_r);
//	// set center to calculated position
//	mBackend->getDataManager()->setCenter(p_r);
//	this->moveManualToolToPosition(p_r);
}

/**Place the global center at the current position of the
 * tooltip of the dominant tool.
 */
void Navigation::centerToTooltip()
{
	ToolPtr tool = mBackend->getToolManager()->getActiveTool();
	Vector3D p_pr = tool->get_prMt().coord(Vector3D(0, 0, tool->getTooltipOffset()));
	Vector3D p_r = mBackend->getPatientService()->get_rMpr().coord(p_pr);

	this->centerToPosition(p_r);
//	// set center to calculated position
//	mBackend->getDataManager()->setCenter(p_r);
}

/**Find the center of all images in the view(wrapper), defined as the mean of
 * all the images center.
 */
Vector3D Navigation::findViewCenter(const std::vector<DataPtr>& images)
{
	return this->findDataCenter(images);
}

/**Find the center of all images, defined as the mean of
 * all the images center.
 */
Vector3D Navigation::findGlobalDataCenter()
{
	std::map<QString,DataPtr> images = mBackend->getPatientService()->getData();
	if (images.empty())
		return Vector3D(0, 0, 0);

	std::map<QString,DataPtr>::iterator iter;
	std::vector<DataPtr> dataVector;

	for (iter = images.begin(); iter != images.end(); ++iter)
	{
		dataVector.push_back(iter->second);
	}
//  std::cout << "findGlobalDataCenter() " << dataVector.size() << std::endl;
	return findDataCenter(dataVector);
}

/**Find the center of the images, defined as the center
 * of the smallest bounding box enclosing the images.
 */
Vector3D Navigation::findDataCenter(std::vector<DataPtr> data)
{
	DoubleBoundingBox3D bb_sigma = findEnclosingBoundingBox(data, Transform3D::Identity());
	return bb_sigma.center();
}

void Navigation::moveManualToolToPosition(Vector3D& p_r)
{
	// move the manual tool to the same position. (this is a side effect... do we want it?)
	ToolPtr manual = mBackend->getToolManager()->getManualTool();
	Vector3D p_pr = mBackend->getPatientService()->get_rMpr().inv().coord(p_r);
	Transform3D prM0t = manual->get_prMt(); // modify old pos in order to keep orientation
	Vector3D t_pr = prM0t.coord(Vector3D(0, 0, manual->getTooltipOffset()));
	Transform3D prM1t = createTransformTranslate(p_pr - t_pr) * prM0t;

	if (!similar(prM1t, prM0t))
		manual->set_prMt(prM1t);
//  std::cout << "center manual tool" << std::endl;
}


} // namespace cx

