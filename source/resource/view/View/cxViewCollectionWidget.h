/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXVIEWCOLLECTIONWIDGET_H_
#define CXVIEWCOLLECTIONWIDGET_H_

#include "cxResourceVisualizationExport.h"

#include "cxView.h"
#include "cxLayoutData.h"
#include <QWidget>


class QGridLayout;

namespace cx
{

/**
 * Widget for displaying Views.
 *
 * This is the main class for displaying visualizations.
 * Add Views using addView(), then add Reps to the Views.
 *
 * \ingroup cx_resource_view
 * \date 2013-11-05
 * \date 2014-09-26
 * \author christiana
 */

class cxResourceVisualization_EXPORT ViewCollectionWidget : public QWidget
{
	Q_OBJECT
public:
	static QPointer<ViewCollectionWidget> createViewWidgetLayout(RenderWindowFactoryPtr factory, QWidget* parent = NULL);
	static QPointer<ViewCollectionWidget> createOptimizedLayout(RenderWindowFactoryPtr factory, QWidget* parent = NULL);

	virtual ~ViewCollectionWidget() {}

	virtual ViewPtr addView(View::Type type, LayoutRegion region) = 0;
	virtual void setOffScreenRenderingAndClear(bool on) = 0;
	virtual bool getOffScreenRendering() const = 0;
	virtual void clearViews() = 0;
	virtual void setModified() = 0;
	virtual void render() = 0;
	virtual void setGridSpacing(int val) = 0;
	virtual void setGridMargin(int val) = 0;
    virtual int getGridSpacing() const = 0;
    virtual int getGridMargin() const = 0;
	virtual void enableContextMenuForViews(bool enable) = 0;

	virtual std::vector<ViewPtr> getViews() = 0;
	/**
	 * Get the position of the Upper Left corner of the view,
	 * given in qt-coordinates of this (the ViewCollectionWidget).
	 */
    virtual QPoint getPosition(ViewPtr view) = 0;

signals:
    void rendered();
protected:
	ViewCollectionWidget(QWidget* parent) : QWidget(parent) {}
};


} // namespace cx

#endif // CXVIEWCOLLECTIONWIDGET_H_
