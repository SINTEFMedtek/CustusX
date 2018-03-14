/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXVIEWLINKINGVIEWWIDGET_H_
#define CXVIEWLINKINGVIEWWIDGET_H_

#include "cxView.h"
#include "cxViewWidget.h"
#include <QPointer>
#include "cxBoundingBox3D.h"
#include "cxViewRepCollection.h"

namespace cx
{

/**
 * \ingroup cx_resource_view_internal
 * \date 2014-09-26
 * \author Christian Askeland
 */
class ViewLinkingViewWidget : public ViewRepCollection
{
public:
	static ViewRepCollectionPtr create(ViewWidget* base, vtkRenderWindowPtr renderWindow)
	{
		boost::shared_ptr<ViewLinkingViewWidget> retval(new ViewLinkingViewWidget(base, renderWindow));
		retval->mSelf = retval;
		return retval;
	}

	ViewLinkingViewWidget(ViewWidget* base, vtkRenderWindowPtr renderWindow) :
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

	virtual ~ViewLinkingViewWidget() {}
	virtual QSize size() const
	{
		if (mBase)
			return mBase->size();
		return QSize(0,0);
	}
	virtual void setZoomFactor(double factor)
	{
		if (mBase)
			mBase->setZoomFactor(factor);
	}
	virtual double getZoomFactor() const
	{
		if (mBase)
			return mBase->getZoomFactor();
		return 0;
	}
	virtual Transform3D get_vpMs() const
	{
		if (mBase)
			return mBase->get_vpMs();
		return Transform3D::Identity();
	}
	virtual DoubleBoundingBox3D getViewport() const
	{
		if (mBase)
			return mBase->getViewport();
		return DoubleBoundingBox3D::zero();
	}
	virtual DoubleBoundingBox3D getViewport_s() const
	{
		if (mBase)
			return mBase->getViewport_s();
		return DoubleBoundingBox3D::zero();
	}

private:
	QPointer<ViewWidget> mBase;
};

} /* namespace cx */
#endif /* CXVIEWLINKINGVIEWWIDGET_H_ */
