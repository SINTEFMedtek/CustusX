// This file is part of SSC,
// a C++ Library supporting Image Guided Therapy Applications.
//
// Copyright (C) 2008- SINTEF Medical Technology
// Copyright (C) 2008- Sonowand AS
//
// SSC is owned by SINTEF Medical Technology and Sonowand AS,
// hereafter named the owners. Each particular piece of code
// is owned by the part that added it to the library.
// SSC source code and binaries can only be used by the owners
// and those with explicit permission from the owners.
// SSC shall not be distributed to anyone else.
//
// SSC is distributed WITHOUT ANY WARRANTY; without even
// the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE.
//
// See sscLicense.txt for more information.

#ifndef SSCVIEW_H_
#define SSCVIEW_H_
#include "sscConfig.h"
#include <boost/shared_ptr.hpp>
#include "vtkForwardDeclarations.h"
#include "sscIndent.h"
class QColor;

#include "sscViewQVTKWidget.h"

#include "sscTransform3D.h"

namespace ssc
{
class DoubleBoundingBox3D;
typedef boost::shared_ptr<class Rep> RepPtr;

/**\brief Base widget for displaying lists of Rep.
 *
 * View inherits from QWidget and thus can be added to a QLayout.
 * It wraps vtkRenderer and vtkRenderWindow, and visualizes a scene
 * with one camera. Add reps to the View in order to visualize them.
 * Although any Rep can be added to a View, it makes most sense
 * dedicate each view for either 2D, 3D or Video display. Most Reps
 * are specialized to one of these three modes.
 *
 * Note: Some special hacks has been introduced in order to share
 * GPU memory between GL contexts (i.e. Views). This is described
 * in the vtkMods folder.
 *
 */
class View
{
public:
	/// type describing the view
	enum Type
	{
		VIEW, VIEW_2D, VIEW_3D, VIEW_REAL_TIME
	};
	View(QWidget *parent, QSize size, const QString& uid = "", const QString& name = "");
	virtual ~View();
	/// \return the View type, indicating display dimension.
	virtual Type getType() const
	{
		return mType;
	}
	virtual void setType(Type type)
	{
		mType = type;
	}
	QString getTypeString() const;
	virtual QString getUid(); ///< Get a views unique id
	virtual QString getName(); ///< Get a views name
	virtual vtkRendererPtr getRenderer() const; ///< Get the renderer used by this \a View.
	virtual void addRep(const RepPtr& rep); ///< Adds and connects a rep to the view
	virtual void setRep(const RepPtr& rep); ///< Remove all other \a Rep objects from this \a View and add the provided Rep to this \a View.
	virtual void removeRep(const RepPtr& rep); ///< Removes and disconnects the rep from the view
	virtual bool hasRep(const RepPtr& rep) const; ///< Checks if the view already have the rep
	virtual std::vector<RepPtr> getReps(); ///< Returns all reps in the view
	virtual void removeReps(); ///< Removes all reps in the view
	virtual void setBackgroundColor(QColor color);
	virtual void render(); ///< render the view contents if vtk-MTimes are changed

	virtual vtkRenderWindowPtr getRenderWindow() const = 0;
	virtual QSize size() const = 0;

	/**
	 * Return the geometry of the view in screen coordinates
	 */
	virtual QRect screenGeometry() const = 0;

	virtual void setZoomFactor(double factor) = 0;
	double getZoomFactor() const;
	Transform3D get_vpMs() const;
	double mmPerPix() const;
	ssc::DoubleBoundingBox3D getViewport() const;
	ssc::DoubleBoundingBox3D getViewport_s() const;

	QWidget *widget() const { return mParent; }
	void forceUpdate() { mMTimeHash = 0; }

protected:
	QSize mSize;
	double mZoomFactor; ///< zoom factor for this view. 1 means that 1m on screen is 1m
	QColor mBackgroundColor;
	unsigned long mMTimeHash; ///< sum of all MTimes in objects rendered
	QString mUid; ///< The view's unique id
	QString mName; ///< The view's name
	vtkRendererPtr mRenderer;
	std::vector<RepPtr> mReps; ///< Storage for internal reps.
	typedef std::vector<RepPtr>::iterator RepsIter; ///< Iterator typedef for the internal rep vector.
	QWidget *mParent;
	Type mType;
};
typedef boost::shared_ptr<View> ViewPtr;

/// Simple 1:1 conflation of SSC Views and Qt Widgets
class ViewWidget : public ViewQVTKWidget, public View
{
Q_OBJECT
	typedef ViewQVTKWidget widget;

public:
	ViewWidget(QWidget *parent = NULL, Qt::WFlags f = 0);
	ViewWidget(const QString& uid, const QString& name = "", QWidget *parent = NULL, Qt::WFlags f = 0); ///< constructor
	virtual ~ViewWidget();

	void print(std::ostream& os);
	virtual void printSelf(std::ostream & os, Indent indent);
	virtual void clear(); ///< Removes everything in the view, inluding reps.

	// Implement pure virtuals in base class
	virtual vtkRenderWindowPtr getRenderWindow() const { return mRenderWindow; } ///< Get the vtkRenderWindow used by this \a View.
	virtual QSize size() const { return widget::size(); }
	virtual void setZoomFactor(double factor);
	virtual QRect screenGeometry() const;

signals:
	void resized(QSize size);
	void mouseMove(int x, int y, Qt::MouseButtons buttons);
	void mousePress(int x, int y, Qt::MouseButtons buttons);
	void mouseRelease(int x, int y, Qt::MouseButtons buttons);
	void mouseWheel(int x, int y, int delta, int orientation, Qt::MouseButtons buttons);
	void shown();
	void focusChange(bool gotFocus, Qt::FocusReason reason);

// old, deprecated signals that pass event objects; this is UNSAFE over thread boundaries!
signals:
	void mouseMoveSignal(QMouseEvent* event);
	void mousePressSignal(QMouseEvent* event);
	void mouseReleaseSignal(QMouseEvent* event);
	void mouseWheelSignal(QWheelEvent*);
	void showSignal(QShowEvent* event);
	void focusInSignal(QFocusEvent* event);

protected:
	vtkRenderWindowPtr mRenderWindow;

private:
	virtual void showEvent(QShowEvent* event);
	virtual void wheelEvent(QWheelEvent*);
	virtual void mouseMoveEvent(QMouseEvent *event);
	virtual void mousePressEvent(QMouseEvent *event);
	virtual void mouseReleaseEvent(QMouseEvent *event);
	virtual void focusInEvent(QFocusEvent* event);
	virtual void resizeEvent(QResizeEvent *event);
	virtual void paintEvent(QPaintEvent *event);
};
typedef boost::shared_ptr<View> ViewPtr;

} // namespace ssc

#endif /*SSCVIEW_H_*/
