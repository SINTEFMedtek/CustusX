/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxViewLinkingViewContainerItem.h"

namespace cx
{

ViewRepCollectionPtr ViewLinkingViewContainerItem::create(ViewItem* base, vtkRenderWindowPtr renderWindow)
{
	boost::shared_ptr<ViewLinkingViewContainerItem> retval(new ViewLinkingViewContainerItem(base, renderWindow));
	retval->mSelf = retval;
	return retval;
}

ViewLinkingViewContainerItem::ViewLinkingViewContainerItem(ViewItem *base, vtkRenderWindowPtr renderWindow) :
	ViewRepCollection(renderWindow, "")
{
	mBase = base;

	connect(base, SIGNAL(resized(QSize)), this, SIGNAL(resized(QSize)));
	connect(base, SIGNAL(mouseMove(int, int, Qt::MouseButtons)), this, SIGNAL(mouseMove(int, int, Qt::MouseButtons)));
	connect(base, SIGNAL(mousePress(int, int, Qt::MouseButtons)), this, SIGNAL(mousePress(int, int, Qt::MouseButtons)));
	connect(base, SIGNAL(mouseRelease(int, int, Qt::MouseButtons)), this, SIGNAL(mouseRelease(int, int, Qt::MouseButtons)));
	connect(base, SIGNAL(mouseWheel(int, int, int, int, Qt::MouseButtons)), this, SIGNAL(mouseWheel(int, int, int, int, Qt::MouseButtons)));
	connect(base, SIGNAL(shown()), this, SIGNAL(shown()));
	connect(base, SIGNAL(focusChange(bool, Qt::FocusReason)), this, SIGNAL(focusChange(bool, Qt::FocusReason)));
	connect(base, SIGNAL(customContextMenuRequestedInGlobalPos(const QPoint &)), this, SIGNAL(customContextMenuRequested(const QPoint &)));

}

ViewLinkingViewContainerItem::~ViewLinkingViewContainerItem() {}
QSize ViewLinkingViewContainerItem::size() const
{
	if (mBase)
		return mBase->size();
	return QSize(0,0);
}
void ViewLinkingViewContainerItem::setZoomFactor(double factor)
{
	if (mBase)
		mBase->setZoomFactor(factor);
}
double ViewLinkingViewContainerItem::getZoomFactor() const
{
	if (mBase)
		return mBase->getZoomFactor();
	return 1;
}
Transform3D ViewLinkingViewContainerItem::get_vpMs() const
{
	if (mBase)
		return mBase->get_vpMs();
	return Transform3D::Identity();
}
DoubleBoundingBox3D ViewLinkingViewContainerItem::getViewport() const
{
	if (mBase)
		return mBase->getViewport();
	return DoubleBoundingBox3D::zero();
}
DoubleBoundingBox3D ViewLinkingViewContainerItem::getViewport_s() const
{
	if (mBase)
		return mBase->getViewport_s();
	return DoubleBoundingBox3D::zero();
}

} /* namespace cx */
