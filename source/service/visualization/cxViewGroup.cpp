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

#include "cxViewGroup.h"

#include <vector>
#include <QtWidgets>

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
