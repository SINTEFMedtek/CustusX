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

#ifndef CXVIEWCONTAINERITEM_H_
#define CXVIEWCONTAINERITEM_H_

#include "cxConfig.h"
#include <boost/shared_ptr.hpp>
#include "vtkForwardDeclarations.h"
#include "cxIndent.h"
#include <QLayoutItem>
#include <QWidget>
#include "cxTransform3D.h"
#include "cxViewRepCollection.h"

class QGridLayout;

namespace cx
{
class DoubleBoundingBox3D;
typedef boost::shared_ptr<class Rep> RepPtr;

/**
 * Adapted from SSC class provided by Sonowand
 *
 * \date 2014-09-26
 * \author Christian Askeland
 * \ingroup cx_resource_visualization_internal
 */
class ViewItem : public QObject, public QLayoutItem
{
Q_OBJECT

public:
	ViewRepCollectionPtr getView() { return mView; }

	ViewItem(QString uid, QString name, QWidget *parent, vtkRenderWindowPtr renderWindow, QRect rect);
	virtual ~ViewItem();

	virtual vtkRenderWindowPtr getRenderWindow()  { return this->getView()->getRenderWindow(); }
	virtual vtkRendererPtr getRenderer()  { return this->getView()->getRenderer(); }
	virtual QSize size() const { return mGeometry.size(); }
	virtual void setZoomFactor(double factor);

	virtual double getZoomFactor() const { return mZoomFactor; }
	virtual Transform3D get_vpMs() const;
	virtual DoubleBoundingBox3D getViewport() const;
	virtual DoubleBoundingBox3D getViewport_s() const;
	virtual void setModified() { return this->getView()->setModified(); }

	// Implementing QLayoutItem's pure virtuals
	virtual Qt::Orientations expandingDirections() const { return Qt::Vertical | Qt::Horizontal; }
	virtual QRect geometry() const { return mGeometry; }
	virtual bool isEmpty() const { return false; }
	virtual QSize maximumSize() const { return mParent->size(); }
	virtual QSize minimumSize() const { return QSize(100, 100); }
	virtual void setGeometry(const QRect &r);
	virtual QSize sizeHint() const { return this->size(); }
	// end QLayoutItem virtuals

	// Force signal output
	void mouseMoveSlot(int x, int y, Qt::MouseButtons buttons) { emit mouseMove(x, y, buttons); }
	void mousePressSlot(int x, int y, Qt::MouseButtons buttons) { emit mousePress(x, y, buttons); }
	void mouseReleaseSlot(int x, int y, Qt::MouseButtons buttons) { emit mouseRelease(x, y, buttons); }
	void mouseWheelSlot(int x, int y, int delta, int orientation, Qt::MouseButtons buttons) { emit mouseWheel(x, y, delta, orientation, buttons); }
	void resizedSlot(QSize size) { emit resized(size); }
	void customContextMenuRequestedGlobalSlot(const QPoint& point) { emit customContextMenuRequestedInGlobalPos(point); }

signals:
	void resized(QSize size);
	void mouseMove(int x, int y, Qt::MouseButtons buttons);
	void mousePress(int x, int y, Qt::MouseButtons buttons);
	void mouseRelease(int x, int y, Qt::MouseButtons buttons);
	void mouseWheel(int x, int y, int delta, int orientation, Qt::MouseButtons buttons);
	void shown();
	void focusChange(bool gotFocus, Qt::FocusReason reason);
	void customContextMenuRequestedInGlobalPos(const QPoint&);

private:
	double mmPerPix() const;

	QRect mGeometry;
	QWidget* mParent;
	double mZoomFactor; ///< zoom factor for this view. 1 means that 1m on screen is 1m
	boost::shared_ptr<class ViewRepCollection> mView;
};

} /* namespace cx */
#endif /* CXVIEWCONTAINERITEM_H_ */
