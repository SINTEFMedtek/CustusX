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

#ifndef CXVIEWCOLLECTIONWIDGETUSINGVIEWCONTAINER_H_
#define CXVIEWCOLLECTIONWIDGETUSINGVIEWCONTAINER_H_

#include "cxResourceVisualizationExport.h"

#include "cxView.h"
#include "cxLayoutData.h"
#include "cxViewCollectionWidget.h"


class QGridLayout;

namespace cx
{

/**
 * Widget for displaying Views, using only a single QVTKWidget/vtkRenderWindow,
 * but one vtkRenderer for each View inside.
 *
 * \date 2014-09-26
 * \author Christian Askeland
 * \ingroup cx_resource_view_internal
 */
class cxResourceVisualization_EXPORT ViewCollectionWidgetUsingViewContainer : public ViewCollectionWidget
{
	Q_OBJECT
public:
	ViewCollectionWidgetUsingViewContainer(QWidget* parent);
    virtual ~ViewCollectionWidgetUsingViewContainer();

	ViewPtr addView(View::Type type, LayoutRegion region);
	void clearViews();
	virtual void setModified();
	virtual void render();
	virtual void setGridSpacing(int val);
	virtual void setGridMargin(int val);
    virtual int getGridSpacing() const;
    virtual int getGridMargin() const;
    virtual std::vector<ViewPtr> getViews();
	virtual LayoutRegion getLayoutRegion(QString view);
    virtual QPoint getPosition(ViewPtr view);

private:
	std::vector<ViewPtr> mViews;
	class ViewContainer* mViewContainer;
	std::map<QString, LayoutRegion> mRegions;
};



} // namespace cx
#endif /* CXVIEWCOLLECTIONWIDGETUSINGVIEWCONTAINER_H_ */
