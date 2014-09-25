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

#ifndef CXVIEWLINKINGVIEWWIDGET_H_
#define CXVIEWLINKINGVIEWWIDGET_H_

#include "cxView.h"
#include "cxViewWidget.h"
#include <QPointer>
#include "cxBoundingBox3D.h"
#include "cxViewRepCollection.h"

namespace cx
{

class ViewLinkingViewWidget : public ViewRepCollection
{
public:
	static ViewRepCollectionPtr create(ViewWidget* base, vtkRenderWindowPtr renderWindow)
	{
		boost::shared_ptr<ViewLinkingViewWidget> retval(new ViewLinkingViewWidget(base, renderWindow));
		retval->mSelf = retval;
		return retval;
	}

	ViewLinkingViewWidget(ViewWidget* base, vtkRenderWindowPtr renderWindow) : ViewRepCollection(renderWindow, "")
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
