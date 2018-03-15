/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXVIEWWIDGET_H_
#define CXVIEWWIDGET_H_

#include "QVTKWidget.h"

#include "cxTransform3D.h"
#include "cxView.h"
#include "cxViewRepCollection.h"
#include "cxViewService.h"

namespace cx
{

/** Simple 1:1 conflation of SSC Views and Qt Widgets
  *
  * \ingroup cx_resource_view_internal
  */
class cxResourceVisualization_EXPORT ViewWidget : public QVTKWidget
{
Q_OBJECT
	typedef QVTKWidget inherited;

public:
	ViewRepCollectionPtr getView();

	ViewWidget(RenderWindowFactoryPtr factory, const QString& uid="", const QString& name = "", QWidget *parent = NULL, Qt::WindowFlags f = 0); ///< constructor
	virtual ~ViewWidget();

	virtual vtkRenderWindowPtr getRenderWindow() { return this->getView()->getRenderWindow(); } ///< Get the vtkRenderWindow used by this \a View.
	virtual QSize size() const { return inherited::size(); }
	virtual void setZoomFactor(double factor);
	virtual vtkRendererPtr getRenderer(); ///< Get the renderer used by this \a View.
	virtual double getZoomFactor() const;
	virtual Transform3D get_vpMs() const;
	virtual double mmPerPix() const;
	virtual DoubleBoundingBox3D getViewport() const;
	virtual DoubleBoundingBox3D getViewport_s() const;

	virtual void setModified() { mView->setModified(); }
	void render();

signals:
	void resized(QSize size);
	void mouseMove(int x, int y, Qt::MouseButtons buttons);
	void mousePress(int x, int y, Qt::MouseButtons buttons);
	void mouseRelease(int x, int y, Qt::MouseButtons buttons);
	void mouseWheel(int x, int y, int delta, int orientation, Qt::MouseButtons buttons);
	void shown();
	void focusChange(bool gotFocus, Qt::FocusReason reason);
	void customContextMenuRequestedInGlobalPos(const QPoint&);

private slots:
	void customContextMenuRequestedSlot(const QPoint& point);
private:
	virtual void showEvent(QShowEvent* event);
	virtual void wheelEvent(QWheelEvent*);
	virtual void mouseMoveEvent(QMouseEvent *event);
	virtual void mousePressEvent(QMouseEvent *event);
	virtual void mouseReleaseEvent(QMouseEvent *event);
	virtual void focusInEvent(QFocusEvent* event);
	virtual void resizeEvent(QResizeEvent *event);
	virtual void paintEvent(QPaintEvent *event);

	double mZoomFactor; ///< zoom factor for this view. 1 means that 1m on screen is 1m
	boost::shared_ptr<class ViewRepCollection> mView;
	unsigned long mMTimeHash; ///< sum of all MTimes in objects rendered

//	SharedOpenGLContextPtr mSharedOpenGLContext;
};

} // namespace cx

#endif /* CXVIEWWIDGET_H_ */
