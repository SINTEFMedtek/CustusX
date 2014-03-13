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

#ifndef SSCVIEWCONTAINER_H_
#define SSCVIEWCONTAINER_H_
#include "sscConfig.h"
#include <boost/shared_ptr.hpp>
#include "vtkForwardDeclarations.h"
#include "sscIndent.h"
#include <QLayoutItem>

#include "QVTKWidget.h"
#include "sscView.h"
#include "sscTransform3D.h"

// Forward declarations
class QGridLayout;

namespace cx
{
class DoubleBoundingBox3D;
typedef boost::shared_ptr<class Rep> RepPtr;

/**
  *
  * \ingroup cx_resource_visualization
  */
class ViewItem : public QObject, public View, public QLayoutItem
{
Q_OBJECT

public:
	ViewItem(QString uid, QString name, QWidget *parent, vtkRenderWindowPtr renderWindow, QRect rect) : QObject(parent), View(parent, rect.size(), uid, name), mSlave(NULL) { mRenderWindow = renderWindow; }
	virtual ~ViewItem() { delete mSlave; }

	// Implement pure virtuals in base class
	virtual vtkRenderWindowPtr getRenderWindow() const { return mRenderWindow; }
	virtual QSize size() const { return mSize; }
	virtual void setZoomFactor(double factor);
	virtual void setSize(QSize size) { mSize = size; emit resized(size); }
	void setRenderer(vtkRendererPtr renderer);

	// Implementing QLayoutItem's pure virtuals
	virtual Qt::Orientations expandingDirections() const { return Qt::Vertical | Qt::Horizontal; }
	virtual QRect geometry() const { return mGeometry; }
	virtual bool isEmpty() const { return false; }
	virtual QSize maximumSize() const { return mParent->size(); }
	virtual QSize minimumSize() const { return QSize(100, 100); }
	virtual void setGeometry(const QRect &r);
	virtual QSize sizeHint() const { return mSize; }
	virtual QWidget *getSlaveWidget() const { return mSlave; }
	virtual void setSlaveWidget(QWidget *slave) { mSlave = slave; mSlave->setParent(mParent);}
	virtual QRect screenGeometry() const;

	// Force signal output
	void mouseMoveSlot(int x, int y, Qt::MouseButtons buttons) { emit mouseMove(x, y, buttons); }
	void mousePressSlot(int x, int y, Qt::MouseButtons buttons) { emit mousePress(x, y, buttons); }
	void mouseReleaseSlot(int x, int y, Qt::MouseButtons buttons) { emit mouseRelease(x, y, buttons); }
	void mouseWheelSlot(int x, int y, int delta, int orientation, Qt::MouseButtons buttons) { emit mouseWheel(x, y, delta, orientation, buttons); }
	void resizedSlot(QSize size) { emit resized(size); }

signals:
	void resized(QSize size);
	void mouseMove(int x, int y, Qt::MouseButtons buttons);
	void mousePress(int x, int y, Qt::MouseButtons buttons);
	void mouseRelease(int x, int y, Qt::MouseButtons buttons);
	void mouseWheel(int x, int y, int delta, int orientation, Qt::MouseButtons buttons);
	void shown();
	void focusChange(bool gotFocus, Qt::FocusReason reason);

private:
	vtkRenderWindowPtr mRenderWindow;
	QRect mGeometry;
	QWidget *mSlave;
};
typedef boost::shared_ptr<ViewItem> ViewItemPtr;

/**
  *
  * \ingroup cx_resource_visualization
  */
class ViewContainerBase
{
public:
	ViewContainerBase(QWidget *parent = NULL);
	virtual ~ViewContainerBase();
	ViewItem *addView(QString uid, int row, int col, int rowSpan = 1, int colSpan = 1, QString name = "");
	virtual void clear();
	void renderAll(); ///< Use this function to render all views at once. Do not call render on each view.

	virtual QGridLayout *getGridLayout() = 0;
	vtkRenderWindowPtr getRenderWindow() { return mRenderWindow; }
	void handleMousePress(const QPoint &pos, const Qt::MouseButtons &buttons); 
	void handleMouseRelease(const QPoint &pos, const Qt::MouseButtons &buttons); 
	void handleMouseMove(const QPoint &pos, const Qt::MouseButtons &buttons); 
	virtual void forcedUpdate();

protected:
	virtual void doRender() = 0;
	ViewItem *mMouseEventTarget;
	vtkRenderWindowPtr mRenderWindow;
	unsigned long mMTimeHash; ///< sum of all MTimes in objects rendered
	virtual void clearBackground() = 0;
	QWidget *mWidget;
};

/**
  *
  * \ingroup cx_resource_visualization
  */
class ViewContainerWidget : public QVTKWidget
{
public:
	ViewContainerWidget(ViewContainerBase *base, QWidget *parent = NULL, Qt::WFlags f = 0);
	void setBase(ViewContainerBase *base) { mBase = base; }
protected:
	typedef QVTKWidget widget;
	
private:
	virtual void paintEvent(QPaintEvent *event);
	virtual void showEvent(QShowEvent* event);
	virtual void wheelEvent(QWheelEvent*);
	virtual void mouseMoveEvent(QMouseEvent *event);
	virtual void mousePressEvent(QMouseEvent *event);
	virtual void mouseReleaseEvent(QMouseEvent *event);
	virtual void focusInEvent(QFocusEvent* event);

	ViewContainerBase *mBase;
};

/** More advanced N:1 combination of SSC Views and Qt Widgets
  *
  * \ingroup cx_resource_visualization
  */
class ViewContainer : public ViewContainerWidget, public ViewContainerBase
{
Q_OBJECT

public:
	ViewContainer(QWidget *parent = NULL, Qt::WFlags f = 0);
	virtual ~ViewContainer();

	virtual QGridLayout *getGridLayout();

protected:
	virtual void clearBackground();
	virtual void doRender();
private:
	virtual void resizeEvent( QResizeEvent *event);
};
typedef boost::shared_ptr<ViewContainer> ViewContainerPtr;

} // namespace cx

#endif /*SSCVIEW_H_*/
