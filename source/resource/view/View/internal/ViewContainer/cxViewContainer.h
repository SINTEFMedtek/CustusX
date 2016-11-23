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

	std::map<QString, vtkRenderWindowPtr> mCachedRenderWindows; // swap between off/onscreen rw's, but dont delete them.
	bool mOffScreenRendering;
	RenderWindowFactoryPtr mRenderWindowFactory;
};
typedef boost::shared_ptr<ViewContainer> ViewContainerPtr;


} /* namespace cx */

#endif /* CXVIEWCONTAINER_H_ */
