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

namespace cx
{

#define ViewLinkingViewWidget_GET_CHECK(METHOD_NAME, defVal) \
	if (mBase)                                               \
		return mBase->METHOD_NAME();                         \
	return defVal;

#define ViewLinkingViewWidget_GET_1_CHECK(METHOD_NAME, defVal, arg0) \
	if (mBase)                                               \
		return mBase->METHOD_NAME(arg0);                         \
	return defVal;

#define ViewLinkingViewWidget_VOID_CHECK(METHOD_NAME)        \
	if (mBase)                                               \
		mBase->METHOD_NAME();

#define ViewLinkingViewWidget_SET_1_CHECK(METHOD_NAME, arg0) \
	if (mBase)                                               \
		mBase->METHOD_NAME(arg0);

class ViewLinkingViewWidget : public View
{
public:
	ViewLinkingViewWidget(ViewWidget* base)
	{
		mBase = base;

		connect(base, SIGNAL(resized(QSize)), this, SIGNAL(resized(QSize)));
		connect(base, SIGNAL(mouseMove(int, int, Qt::MouseButtons)), this, SIGNAL(mouseMove(int, int, Qt::MouseButtons)));
		connect(base, SIGNAL(mousePress(int, int, Qt::MouseButtons)), this, SIGNAL(mousePress(int, int, Qt::MouseButtons)));
		connect(base, SIGNAL(mouseRelease(int, int, Qt::MouseButtons)), this, SIGNAL(mouseRelease(int, int, Qt::MouseButtons)));
		connect(base, SIGNAL(mouseWheel(int, int, int, int, Qt::MouseButtons)), this, SIGNAL(mouseWheel(int, int, int, int, Qt::MouseButtons)));
		connect(base, SIGNAL(shown()), this, SIGNAL(shown()));
		connect(base, SIGNAL(focusChange(bool, Qt::FocusReason)), this, SIGNAL(focusChange(bool, Qt::FocusReason)));
		connect(base, SIGNAL(customContextMenuRequested(const QPoint &)), this, SIGNAL(customContextMenuRequested(const QPoint &)));

	}

	virtual ~ViewLinkingViewWidget() {}
	virtual Type getType() const { ViewLinkingViewWidget_GET_CHECK(getType, VIEW);  }
//	virtual void setType(Type type) { ViewLinkingViewWidget_SET_1_CHECK(setType, type); }
	virtual QString getTypeString() const { ViewLinkingViewWidget_GET_CHECK(getTypeString, ""); }
	virtual QString getUid() { ViewLinkingViewWidget_GET_CHECK(getUid, ""); }
	virtual QString getName()  { ViewLinkingViewWidget_GET_CHECK(getName, ""); }
	virtual vtkRendererPtr getRenderer() const  { ViewLinkingViewWidget_GET_CHECK(getRenderer, vtkRendererPtr()); }
	virtual void addRep(const RepPtr& rep)  { ViewLinkingViewWidget_SET_1_CHECK(addRep, rep); }
//	virtual void setRep(const RepPtr& rep)  { ViewLinkingViewWidget_SET_1_CHECK(setRep, rep); }
	virtual void removeRep(const RepPtr& rep)  { ViewLinkingViewWidget_SET_1_CHECK(removeRep, rep); }
	virtual bool hasRep(const RepPtr& rep) const  { ViewLinkingViewWidget_GET_1_CHECK(hasRep, false, rep); }
	virtual std::vector<RepPtr> getReps() { ViewLinkingViewWidget_GET_CHECK(getReps, std::vector<RepPtr>()); }
	virtual void removeReps() { ViewLinkingViewWidget_VOID_CHECK(removeReps); }
	virtual void setBackgroundColor(QColor color) { ViewLinkingViewWidget_SET_1_CHECK(setBackgroundColor, color); }
	virtual void render() { ViewLinkingViewWidget_VOID_CHECK(render); }
	virtual vtkRenderWindowPtr getRenderWindow() const { ViewLinkingViewWidget_GET_CHECK(getRenderWindow, vtkRenderWindowPtr()); }
	virtual QSize size() const { ViewLinkingViewWidget_GET_CHECK(size, QSize(0,0)); }
//	virtual QRect screenGeometry() const{ ViewLinkingViewWidget_GET_CHECK(screenGeometry, QRect()); }
	virtual void setZoomFactor(double factor)  { ViewLinkingViewWidget_SET_1_CHECK(setZoomFactor, factor); }
	virtual double getZoomFactor() const { ViewLinkingViewWidget_GET_CHECK(getZoomFactor, 0); }
	virtual Transform3D get_vpMs() const { ViewLinkingViewWidget_GET_CHECK(get_vpMs, Transform3D::Identity()); }
//	virtual double mmPerPix() const { ViewLinkingViewWidget_GET_CHECK(mmPerPix, 0); }
//	virtual double heightMM() const { ViewLinkingViewWidget_GET_CHECK(heightMM, 0); }
	virtual DoubleBoundingBox3D getViewport() const { ViewLinkingViewWidget_GET_CHECK(getViewport, DoubleBoundingBox3D::zero()); }
	virtual DoubleBoundingBox3D getViewport_s() const { ViewLinkingViewWidget_GET_CHECK(getViewport_s, DoubleBoundingBox3D::zero()); }
//	virtual QWidget *widget() const   { return mBase; }
	virtual void forceUpdate()  { ViewLinkingViewWidget_VOID_CHECK(forceUpdate); }

private:
	QPointer<ViewWidget> mBase;
};

} /* namespace cx */
#endif /* CXVIEWLINKINGVIEWWIDGET_H_ */
