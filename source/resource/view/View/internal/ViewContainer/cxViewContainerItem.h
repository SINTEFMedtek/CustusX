/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
 * \ingroup cx_resource_view_internal
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
