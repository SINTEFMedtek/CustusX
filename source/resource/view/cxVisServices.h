/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXVISSERVICES_H
#define CXVISSERVICES_H

#include "cxResourceVisualizationExport.h"

#include "cxCoreServices.h"

namespace cx
{

typedef boost::shared_ptr<class VisServices> VisServicesPtr;
typedef boost::shared_ptr<class ViewService> ViewServicePtr;

/**
 * Convenience class combining all services used by visualization.
 *
 * \ingroup cx_resource_view
 *
 * \date Nov 14 2014
 * \author Ole Vegard Solberg, SINTEF
 */
class cxResourceVisualization_EXPORT VisServices : public CoreServices
{
public:
	static VisServicesPtr create(ctkPluginContext* context);
	VisServices(ctkPluginContext* context);
	static VisServicesPtr getNullObjects();

	ViewServicePtr view() { return mViewService;}

protected:
	VisServices();
	ViewServicePtr mViewService;
};


}

#endif // CXVISSERVICES_H
