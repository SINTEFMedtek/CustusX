/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXVIEWCONTAINER_H_
#define CXVIEWCONTAINER_H_

#include "cxResourceVisualizationExport.h"

#include "cxConfig.h"
#include <boost/shared_ptr.hpp>
#include "vtkForwardDeclarations.h"
#include "QVTKWidget.h"
#include "cxLayoutData.h"
#include "cxViewService.h"

class QGridLayout;

namespace cx
{
class ViewItem;
typedef boost::shared_ptr<class MultiViewCache> MultiViewCachePtr;

/**
 * More advanced N:1 combination of SSC Views and Qt Widgets
 * Adapted from SSC class provided by Sonowand
 *
 * \date 2014-09-26
 * \author Christian Askeland
 * \ingroup cx_resource_view_internal
 */
class cxResourceVisualization_EXPORT ViewContainer : public QVTKWidget
{
	Q_OBJECT
	typedef QVTKWidget inherited_widget;

public:
	ViewContainer(RenderWindowFactoryPtr factory, QWidget *parent = NULL, Qt::WindowFlags f = 0);
	virtual ~ViewContainer();

	ViewItem *addView(QString uid, LayoutRegion region, QString name = "");
	virtual void clear();
	void renderAll(); ///< Use this function to render all views at once. Do not call render on each view.

	virtual void setOffScreenRenderingAndClear(bool on);
	virtual bool getOffScreenRendering() const;

	vtkRenderWindowPtr getRenderWindow() { return mRenderWindow; }
	virtual void setModified();

	virtual QGridLayout *getGridLayout();

private:
	virtual void paintEvent(QPaintEvent *event);
	virtual void showEvent(QShowEvent* event);
	virtual void wheelEvent(QWheelEvent*);
	virtual void mouseMoveEvent(QMouseEvent *event);
	virtual void mousePressEvent(QMouseEvent *event);
	virtual void mouseReleaseEvent(QMouseEvent *event);
	virtual void focusInEvent(QFocusEvent* event);
private slots:
	void customContextMenuRequestedSlot(const QPoint& point);

protected:
	ViewItem *mMouseEventTarget;
	vtkRenderWindowPtr mRenderWindow;
	unsigned long mMTimeHash; ///< sum of all MTimes in objects rendered
	virtual void doRender();
	ViewItem* getViewItem(int index);

private:
	virtual void resizeEvent( QResizeEvent *event);
	void initializeRenderWindow();
	void addBackgroundRenderer(vtkRenderWindowPtr rw);
	QPoint convertToItemSpace(const QPoint &pos, ViewItem* item) const;
	ViewItem* findViewItem(const QPoint &pos);

//	std::map<QString, vtkRenderWindowPtr> mCachedRenderWindows; // swap between off/onscreen rw's, but dont delete them.
	bool mOffScreenRendering;
	RenderWindowFactoryPtr mRenderWindowFactory;
};
typedef boost::shared_ptr<ViewContainer> ViewContainerPtr;


} /* namespace cx */

#endif /* CXVIEWCONTAINER_H_ */
