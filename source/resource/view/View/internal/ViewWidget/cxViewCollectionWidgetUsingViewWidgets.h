/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXVIEWCOLLECTIONWIDGETUSINGVIEWWIDGETS_H_
#define CXVIEWCOLLECTIONWIDGETUSINGVIEWWIDGETS_H_

#include "cxResourceVisualizationExport.h"

#include "cxView.h"
#include "cxLayoutData.h"
#include "cxViewCache.h"
#include "cxViewWidget.h"
#include "cxViewCollectionWidget.h"

class QGridLayout;

namespace cx
{

typedef boost::shared_ptr<class MultiViewCache> MultiViewCachePtr;

/**
 * Widget for displaying Views, Containing a QGridLayout of QVTKWidgets,
 * one for each view.
 *
 * \ingroup cx_resource_view_internal
 * \date 2013-11-05
 * \author Christian Askeland
 */
class cxResourceVisualization_EXPORT LayoutWidgetUsingViewWidgets : public ViewCollectionWidget
{
	Q_OBJECT
public:
	LayoutWidgetUsingViewWidgets(RenderWindowFactoryPtr factory, QWidget *parent);
    virtual ~LayoutWidgetUsingViewWidgets();

	virtual ViewPtr addView(View::Type type, LayoutRegion region);
	virtual void setOffScreenRenderingAndClear(bool on);
	virtual bool getOffScreenRendering() const;
	virtual void clearViews();
	virtual void setModified();
	virtual void render();
	virtual void setGridSpacing(int val);
	virtual void setGridMargin(int val);
    virtual int getGridSpacing() const;
    virtual int getGridMargin() const;
    virtual std::vector<ViewPtr> getViews();
    virtual QPoint getPosition(ViewPtr view);
	virtual void enableContextMenuForViews(bool enable);

protected:
	std::vector<ViewWidget*> mViews;

private:
    ViewWidget* WidgetFromView(ViewPtr view);

	MultiViewCachePtr mViewCache;
	QGridLayout* mLayout; ///< the layout
	bool mOffScreenRendering;
};

} // namespace cx

#endif /* CXVIEWCOLLECTIONWIDGETUSINGVIEWWIDGETS_H_ */
