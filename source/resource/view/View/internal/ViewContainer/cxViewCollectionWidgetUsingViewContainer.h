/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
	ViewCollectionWidgetUsingViewContainer(RenderWindowFactoryPtr factory, QWidget* parent);
    virtual ~ViewCollectionWidgetUsingViewContainer();

	ViewPtr addView(View::Type type, LayoutRegion region);
	virtual void setOffScreenRenderingAndClear(bool on);
	virtual bool getOffScreenRendering() const;
	void clearViews();
	virtual void setModified();
	virtual void render();
	virtual void setGridSpacing(int val);
	virtual void setGridMargin(int val);
    virtual int getGridSpacing() const;
    virtual int getGridMargin() const;
    virtual std::vector<ViewPtr> getViews();
    virtual QPoint getPosition(ViewPtr view);
	virtual void enableContextMenuForViews(bool enable);

private:
	std::vector<ViewPtr> mViews;
	class ViewContainer* mViewContainer;
};



} // namespace cx
#endif /* CXVIEWCOLLECTIONWIDGETUSINGVIEWCONTAINER_H_ */
