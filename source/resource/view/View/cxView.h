/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXVIEW_H_
#define CXVIEW_H_

#include <QObject>
#include "sscConfig.h"
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include "vtkForwardDeclarations.h"
#include "cxIndent.h"
#include <QSize>
class QColor;

#include "cxTransform3D.h"

namespace cx
{
class DoubleBoundingBox3D;
typedef boost::shared_ptr<class Rep> RepPtr;

/**
 * Base widget for displaying representations (Rep).
 *
 * Use a ViewCollectionWidget to create Views.
 *
 * A View represents one visualization scene, wrapping a
 * vtkRenderer, vtkCamera and a vtkRenderWindow. The vtkRenderWindow
 * might be shared with other Views.
 *
 * Add reps to the View in order to visualize them. Although any Rep
 * can be added to a View, it makes most sense to dedicate each view
 * for either 2D, 3D or Video display. Most Reps are specialized to
 * one of these three modes.
 *
 *
 * \ingroup cx_resource_view
 */
class View : public QObject
{
	Q_OBJECT
public:
	enum Type
	{
		VIEW, VIEW_2D, VIEW_3D, VIEW_REAL_TIME
	};

	virtual ~View() {}

	virtual Type getType() const = 0;
	virtual QString getTypeString() const = 0;
	virtual QString getUid() = 0; ///< Get a views unique id
	virtual QString getName() = 0; ///< Get a views name

	virtual void addRep(const RepPtr& rep) = 0; ///< Adds and connects a rep to the view
	virtual void removeRep(const RepPtr& rep) = 0; ///< Removes and disconnects the rep from the view
	virtual bool hasRep(const RepPtr& rep) const = 0; ///< Checks if the view already have the rep
	virtual std::vector<RepPtr> getReps() = 0; ///< Returns all reps in the view
	virtual void removeReps() = 0; ///< Removes all reps in the view

	virtual vtkRendererPtr getRenderer() const = 0; ///< Get the renderer used by this \a View.
	virtual vtkRenderWindowPtr getRenderWindow() const = 0;
	virtual void setModified() = 0;
	virtual void setBackgroundColor(QColor color) = 0;
	virtual QSize size() const = 0;
	virtual void setZoomFactor(double factor) = 0;
	virtual double getZoomFactor() const = 0;
	virtual Transform3D get_vpMs() const = 0;
	virtual DoubleBoundingBox3D getViewport() const = 0;
	virtual DoubleBoundingBox3D getViewport_s() const = 0;

signals:
	void resized(QSize size);
	void mouseMove(int x, int y, Qt::MouseButtons buttons);
	void mousePress(int x, int y, Qt::MouseButtons buttons);
	void mouseRelease(int x, int y, Qt::MouseButtons buttons);
	void mouseWheel(int x, int y, int delta, int orientation, Qt::MouseButtons buttons);
	void shown();
	void focusChange(bool gotFocus, Qt::FocusReason reason);
	void customContextMenuRequested(const QPoint&);
};
typedef boost::shared_ptr<View> ViewPtr;

} // namespace cx

#endif /*CXVIEW_H_*/
