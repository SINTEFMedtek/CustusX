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

#include "cxTrackingService.h"
#include "cxViewWrapper2D.h"
#include "cxCameraControl.h"
#include "cxData.h"
#include "cxViewWrapper.h"
#include "cxManualTool.h"
#include "cxVolumeHelpers.h"
#include "cxTypeConversions.h"
#include "cxCoreServices.h"
#include "cxCameraStyle.h"
#include "cxPatientModelService.h"
#include "cxImage.h"
#include "cxMesh.h"
#include "cxTrackedStream.h"
#include "cxActiveData.h"
#include "cxLogger.h"

namespace cx
{


ViewGroup::ViewGroup(CoreServicesPtr backend, QString uid)
{
	CX_LOG_DEBUG() << "ViewGroup constr";
	mBackend = backend;
	mViewGroupData.reset(new ViewGroupData(backend, uid));
	mCameraStyle.reset(new CameraStyle(mBackend, mViewGroupData));

	connect(mViewGroupData.get(), &ViewGroupData::optionsChanged, this, &ViewGroup::optionChangedSlot);
	this->optionChangedSlot();
}

ViewGroup::~ViewGroup()
{
}

void ViewGroup::optionChangedSlot()
{
	mCameraStyle->setCameraStyle(mViewGroupData->getOptions().mCameraStyle);
}

/**Add one view wrapper and setup the necessary connections.
 */
void ViewGroup::addView(ViewWrapperPtr wrapper, SharedOpenGLContextPtr sharedOpenGLContext)
{
	//mViewGroupData->setSharedOpenGLContext(sharedOpenGLContext);
	wrapper->setSharedOpenGLContext(sharedOpenGLContext);
	mViews.push_back(wrapper->getView());
	mViewWrappers.push_back(wrapper);

	// add state
	wrapper->setViewGroup(mViewGroupData);
	mCameraStyle->addView(wrapper->getView());

	View* view = wrapper->getView().get();
	// connect signals
//	connect(view, SIGNAL(mousePress(int, int, Qt::MouseButtons)), this, SLOT(activateManualToolSlot()));
	connect(view, SIGNAL(mousePress(int, int, Qt::MouseButtons)), this, SLOT(mouseClickInViewGroupSlot()));
	connect(view, SIGNAL(focusChange(bool, Qt::FocusReason)), this, SLOT(mouseClickInViewGroupSlot()));
}

void ViewGroup::removeViews()
{
	for (unsigned i = 0; i < mViewWrappers.size(); ++i)
	{
		ViewWrapperPtr wrapper = mViewWrappers[i];
		View* view = wrapper->getView().get();

//		disconnect(view, SIGNAL(mousePress(int, int, Qt::MouseButtons)), this, SLOT(activateManualToolSlot()));
		disconnect(view, SIGNAL(mousePress(int, int, Qt::MouseButtons)), this, SLOT(mouseClickInViewGroupSlot()));
		disconnect(view, SIGNAL(focusChange(bool, Qt::FocusReason)), this, SLOT(mouseClickInViewGroupSlot()));
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

void ViewGroup::mouseClickInViewGroupSlot()
{
	std::vector<ImagePtr> images = mViewGroupData->getImages(DataViewProperties::createFull());
	std::vector<MeshPtr> meshes = mViewGroupData->getMeshes(DataViewProperties::createFull());
	std::vector<TrackedStreamPtr> trackedStreams = mViewGroupData->getTrackedStreams(DataViewProperties::createFull());
	ActiveDataPtr activeData = mBackend->patient()->getActiveData();

	if(shouldUpdateActiveData(activeData->getActive<Mesh>(), meshes))
		activeData->setActive(meshes.front()->getUid());
	if(shouldUpdateActiveData(activeData->getActive<Image>(), images))
		activeData->setActive(images.front()->getUid());
	if(shouldUpdateActiveData(activeData->getActive<TrackedStream>(), trackedStreams))
		activeData->setActive(trackedStreams.front()->getUid());

	View* view = static_cast<View*>(this->sender());
	if (view && mActiveView)
		mActiveView->set(view->getUid());
}

template<class T>
bool ViewGroup::shouldUpdateActiveData(T activeData, std::vector<T> datas) const
{
	bool activeDataExistsInGroup = std::find(datas.begin(), datas.end(), activeData) != datas.end();
	return !activeDataExistsInGroup && !datas.empty();
}

std::vector<ViewPtr> ViewGroup::getViews() const
{
	return mViews;
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
