/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXVIEWLINKINGVIEWCONTAINERITEM_H
#define CXVIEWLINKINGVIEWCONTAINERITEM_H

#include "cxView.h"
#include "cxViewWidget.h"
#include <QPointer>
#include "cxBoundingBox3D.h"
#include "cxViewRepCollection.h"
#include "cxViewContainerItem.h"

namespace cx
{

/**
 * \date 2014-09-26
 * \author Christian Askeland
 * \ingroup cx_resource_view_internal
 */
class ViewLinkingViewContainerItem : public ViewRepCollection
{
public:
	static ViewRepCollectionPtr create(ViewItem* base, vtkRenderWindowPtr renderWindow);
	ViewLinkingViewContainerItem(ViewItem* base, vtkRenderWindowPtr renderWindow);
	virtual ~ViewLinkingViewContainerItem();
	virtual QSize size() const;
	virtual void setZoomFactor(double factor);
	virtual double getZoomFactor() const;
	virtual Transform3D get_vpMs() const;
	virtual DoubleBoundingBox3D getViewport() const;
	virtual DoubleBoundingBox3D getViewport_s() const;

private:
	QPointer<ViewItem> mBase;
};

} /* namespace cx */

#endif // CXVIEWLINKINGVIEWCONTAINERITEM_H
