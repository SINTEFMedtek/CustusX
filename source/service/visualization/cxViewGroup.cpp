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
#include "cxReporter.h"
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
