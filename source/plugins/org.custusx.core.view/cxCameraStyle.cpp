/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxCameraStyle.h"

#include <QAction>
#include "cxLogger.h"
#include "cxView.h"
#include "cxViewGroup.h"
#include "cxLogger.h"

namespace cx
{

CameraStyle::CameraStyle(CoreServicesPtr backend, ViewGroupDataPtr viewGroupData) :
	mCameraStyle(cstDEFAULT_STYLE),
	mBackend(backend),
	mViewGroupData(viewGroupData)
{
}

void CameraStyle::addView(ViewPtr view)
{
	if (!view || view->getType()!=View::VIEW_3D)
		return;

	CameraStyleForViewPtr style(new CameraStyleForView(mBackend));
	style->setView(view);
	style->setCameraStyle(CameraStyleData(mCameraStyle));
	mViews.push_back(style);
}

void CameraStyle::clearViews()
{
	mViews.clear();
}

CameraStyleData CameraStyle::getCameraStyle() const
{
	return mCameraStyle;
}

void CameraStyle::setCameraStyle(CameraStyleData style)
{
	if (mCameraStyle == style)
		return;

	for (unsigned i=0; i<mViews.size(); ++i)
		mViews[i]->setCameraStyle(style);
	mCameraStyle = style;

	emit cameraStyleChanged();
}



}//namespace cx
