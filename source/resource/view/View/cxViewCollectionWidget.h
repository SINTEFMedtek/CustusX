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
	static QPointer<ViewCollectionWidget> createViewWidgetLayout(ViewServicePtr viewService, QWidget* parent = NULL);
	static QPointer<ViewCollectionWidget> createOptimizedLayout(ViewServicePtr viewService, QWidget* parent = NULL);

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
