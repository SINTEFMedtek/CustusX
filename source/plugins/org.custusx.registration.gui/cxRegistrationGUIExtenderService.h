/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXREGISTRATIONGUIEXTENDERSERVICE_H_
#define CXREGISTRATIONGUIEXTENDERSERVICE_H_

#include "org_custusx_registration_gui_Export.h"
#include "cxGUIExtenderService.h"
class ctkPluginContext;

namespace cx
{

/**
 * Implementation of Registration GUI service.
 *
 * \ingroup org_custusx_registration_gui
 *
 * \date Sep 08 2014
 * \author Ole Vegard Solberg, SINTEF
 */
class org_custusx_registration_gui_EXPORT RegistrationGUIExtenderService : public GUIExtenderService
{
	Q_INTERFACES(cx::GUIExtenderService)
public:
	RegistrationGUIExtenderService(ctkPluginContext *context);
	virtual ~RegistrationGUIExtenderService();

	std::vector<CategorizedWidget> createWidgets() const;

private:
  ctkPluginContext* mContext;

};
//typedef boost::shared_ptr<ExampleGUIExtenderService> ExampleGUIExtenderServicePtr;

} /* namespace cx */

#endif /* CXREGISTRATIONGUIEXTENDERSERVICE_H_ */

