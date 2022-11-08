/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXTRACKINGSHAPEGUIEXTENDERSERVICE_H_
#define CXTRACKINGSHAPEGUIEXTENDERSERVICE_H_

#include "cxGUIExtenderService.h"
#include "org_custusx_tracking_shape_Export.h"
class ctkPluginContext;

namespace cx
{

/**
 * Implementation of TrackingShape service.
 *
 * \ingroup org_custusx_tracking_shape
 *
 * \date 2022-11-08
 * \author Ole Vegard Solberg
 */
class org_custusx_tracking_shape_EXPORT TrackingShapeGUIExtenderService : public GUIExtenderService
{
	Q_INTERFACES(cx::GUIExtenderService)
public:
	TrackingShapeGUIExtenderService(ctkPluginContext *context);
	virtual ~TrackingShapeGUIExtenderService() {};

	std::vector<CategorizedWidget> createWidgets() const;

private:
  ctkPluginContext* mContext;

};
typedef boost::shared_ptr<TrackingShapeGUIExtenderService> TrackingShapeGUIExtenderServicePtr;

} /* namespace cx */

#endif /* CXTRACKINGSHAPEGUIEXTENDERSERVICE_H_ */

