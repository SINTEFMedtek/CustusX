// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.

#include "cxNavigation.h"
#include "cxVisualizationServiceBackend.h"
#include "cxData.h"
#include "cxBoundingBox3D.h"
#include "cxDataManager.h"
#include "cxToolManager.h"
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
		mBackend->getDataManager()->setCenter(p_r);
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
	if (mBackend->getDataManager()->getData().empty())
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
	ToolPtr tool = mBackend->getToolManager()->getDominantTool();
	Vector3D p_pr = tool->get_prMt().coord(Vector3D(0, 0, tool->getTooltipOffset()));
	Vector3D p_r = mBackend->getDataManager()->get_rMpr().coord(p_pr);

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
	DataManager::DataMap images = mBackend->getDataManager()->getData();
	if (images.empty())
		return Vector3D(0, 0, 0);

	DataManager::DataMap::iterator iter;
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
	ManualToolPtr manual = mBackend->getToolManager()->getManualTool();
	Vector3D p_pr = mBackend->getDataManager()->get_rMpr().inv().coord(p_r);
	Transform3D prM0t = manual->get_prMt(); // modify old pos in order to keep orientation
	Vector3D t_pr = prM0t.coord(Vector3D(0, 0, manual->getTooltipOffset()));
	Transform3D prM1t = createTransformTranslate(p_pr - t_pr) * prM0t;

	if (!similar(prM1t, prM0t))
		manual->set_prMt(prM1t);
//  std::cout << "center manual tool" << std::endl;
}


} // namespace cx

