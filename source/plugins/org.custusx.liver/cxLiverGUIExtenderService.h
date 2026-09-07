/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXLIVERGUIEXTENDERSERVICE_H_
#define CXLIVERGUIEXTENDERSERVICE_H_

#include "cxGUIExtenderService.h"
#include "org_custusx_liver_Export.h"
class ctkPluginContext;

namespace cx
{

class org_custusx_liver_EXPORT LiverGUIExtenderService : public GUIExtenderService
{
	Q_INTERFACES(cx::GUIExtenderService)
public:
	LiverGUIExtenderService(ctkPluginContext *context);
	virtual ~LiverGUIExtenderService() {}

	std::vector<CategorizedWidget> createWidgets() const;

private:
	ctkPluginContext* mContext;
};
typedef boost::shared_ptr<LiverGUIExtenderService> LiverGUIExtenderServicePtr;

} /* namespace cx */

#endif /* CXLIVERGUIEXTENDERSERVICE_H_ */
