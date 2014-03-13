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

#include "cxViewGroup.h"

#include <vector>
#include <QtGui>
#include <vtkRenderWindow.h>
#include "cxView.h"
#include "cxSliceProxy.h"
#include "cxSlicerRepSW.h"
#include "cxToolRep2D.h"
#include "cxUtilHelpers.h"
#include "cxSlicePlanes3DRep.h"
#include "cxMessageManager.h"
#include "cxDataManager.h"
#include "cxToolManager.h"
#include "cxViewWrapper2D.h"
#include "cxViewManager.h"
#include "cxCameraControl.h"
#include "cxData.h"
#include "cxViewWrapper.h"
#include "cxManualTool.h"
#include "cxVolumeHelpers.h"
#include "cxTypeConversions.h"
#include "cxVisualizationServiceBackend.h"
#include "cxCameraStyle.h"

namespace cx
{

Navigation::Navigation(VisualizationServiceBackendPtr backend) :
	mBackend(backend)
{

}

/**Place the global center to the center of the image.
 */
void Navigation::centerToData(DataPtr image)
{
	if (!image)
		return;
	Vector3D p_r = image->get_rMd().coord(image->boundingBox().center());

	// set center to calculated position
	mBackend->getDataManager()->setCenter(p_r);
}

/**Place the global center to the mean center of
 * all the images in a view(wrapper).
 */
void Navigation::centerToView(const std::vector<DataPtr>& images)
{
	Vector3D p_r = findViewCenter(images);
	std::cout << "center ToView: " << images.size() << " - " << p_r << std::endl;

	// set center to calculated position
	mBackend->getDataManager()->setCenter(p_r);
}

/**Place the global center to the mean center of
 * all the loaded images.
 */
void Navigation::centerToGlobalDataCenter()
{
	if (mBackend->getDataManager()->getData().empty())
		return;

	Vector3D p_r = this->findGlobalDataCenter();

	// set center to calculated position
	mBackend->getDataManager()->setCenter(p_r);
}

/**Place the global center at the current position of the
 * tooltip of the dominant tool.
 */
void Navigation::centerToTooltip()
{
	ToolPtr tool = mBackend->getToolManager()->getDominantTool();
	Vector3D p_pr = tool->get_prMt().coord(Vector3D(0, 0, tool->getTooltipOffset()));
	Vector3D p_r = mBackend->getDataManager()->get_rMpr().coord(p_pr);

	// set center to calculated position
	mBackend->getDataManager()->setCenter(p_r);
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
//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------

ViewGroup::ViewGroup(VisualizationServiceBackendPtr backend)
{
	mBackend = backend;
	mCameraStyle.reset(new CameraStyle(mBackend));

	mViewGroupData.reset(new ViewGroupData(backend));
}

ViewGroup::~ViewGroup()
{
}

/**Add one view wrapper and setup the necessary connections.
 */
void ViewGroup::addView(ViewWrapperPtr wrapper)
{
	mViews.push_back(wrapper->getView());
	mViewWrappers.push_back(wrapper);

	// add state
	wrapper->setViewGroup(mViewGroupData);
	mCameraStyle->addView(wrapper->getView());

	// connect signals
	connect(wrapper->getView(), SIGNAL(mousePressSignal(QMouseEvent*)), this, SLOT(activateManualToolSlot()));
	connect(wrapper->getView(), SIGNAL(mousePressSignal(QMouseEvent*)), this, SLOT(mouseClickInViewGroupSlot()));
	connect(wrapper->getView(), SIGNAL(focusInSignal(QFocusEvent*)), this, SLOT(mouseClickInViewGroupSlot()));
}

void ViewGroup::removeViews()
{
	for (unsigned i = 0; i < mViewWrappers.size(); ++i)
	{
		ViewWrapperPtr wrapper = mViewWrappers[i];

		disconnect(wrapper->getView(), SIGNAL(mousePressSignal(QMouseEvent*)), this, SLOT(activateManualToolSlot()));
		disconnect(wrapper->getView(), SIGNAL(mousePressSignal(QMouseEvent*)), this, SLOT(mouseClickInViewGroupSlot()));
		disconnect(wrapper->getView(), SIGNAL(focusInSignal(QFocusEvent*)), this, SLOT(mouseClickInViewGroupSlot()));
	}

	mViews.clear();
	mViewWrappers.clear();
	mCameraStyle->clearViews();
}

ViewWrapperPtr ViewGroup::getViewWrapperFromViewUid(QString viewUid)
{
	for (unsigned i = 0; i < mViewWrappers.size(); ++i)
	{
		if (mViewWrappers[i]->getView()->getUid() == viewUid)
			return mViewWrappers[i];
	}
	return ViewWrapperPtr();
}

void ViewGroup::syncOrientationMode(SyncedValuePtr val)
{
	for (unsigned i = 0; i < mViewWrappers.size(); ++i)
	{
		mViewWrappers[i]->setOrientationMode(val);
	}
}

void ViewGroup::mouseClickInViewGroupSlot()
{
	std::vector<ImagePtr> images = mViewGroupData->getImages(DataViewProperties::createFull());
	if (!images.empty())
	{
		if (!std::count(images.begin(), images.end(), mBackend->getDataManager()->getActiveImage()))
		{
			mBackend->getDataManager()->setActiveImage(images.front());
		}
	}

	ViewWidgetQPtr view = static_cast<ViewWidget*>(this->sender());
	if (view && mActiveView)
		mActiveView->set(view->getUid());
}

std::vector<ViewWidgetQPtr> ViewGroup::getViews() const
{
	return mViews;
}

void ViewGroup::activateManualToolSlot()
{
	mBackend->getToolManager()->dominantCheckSlot();
}

void ViewGroup::initializeActiveView(SyncedValuePtr val)
{
	mActiveView = val;
}


void ViewGroup::addXml(QDomNode& dataNode)
{
	mViewGroupData->addXml(dataNode);
}

void ViewGroup::clearPatientData()
{
	mViewGroupData->clearData();
}

void ViewGroup::parseXml(QDomNode dataNode)
{
	mViewGroupData->parseXml(dataNode);
}

bool ViewGroup::contains3DView() const
{
	for (unsigned j = 0; j < mViews.size(); ++j)
	{
		if (mViews[j] && (mViews[j]->getType()==View::VIEW_3D))
			return true;
	}
	return false;
}

} //cx
