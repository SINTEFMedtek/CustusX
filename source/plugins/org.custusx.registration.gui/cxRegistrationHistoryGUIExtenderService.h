/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXREGISTRATIONHISTORYGUIEXTENDERSERVICE_H
#define CXREGISTRATIONHISTORYGUIEXTENDERSERVICE_H

#include "org_custusx_registration_gui_Export.h"
#include "cxGUIExtenderService.h"
#include "cxRegServices.h"
class ctkPluginContext;

namespace cx
{

/**
 * Implementation of RegistrationHistory GUI service.
 *
 * \ingroup org_custusx_registration_gui
 *
 * \date Oct 14 2014
 * \author Ole Vegard Solberg, SINTEF
 */
class org_custusx_registration_gui_EXPORT RegistrationHistoryGUIExtenderService : public GUIExtenderService
{
	Q_INTERFACES(cx::GUIExtenderService)
public:
	RegistrationHistoryGUIExtenderService(RegServicesPtr services);
	virtual ~RegistrationHistoryGUIExtenderService();

	virtual std::vector<CategorizedWidget> createWidgets() const;	
	std::vector<QToolBar *> createToolBars() const;

private:
	RegServicesPtr mServices;
};

} /* namespace cx */


#endif // CXREGISTRATIONHISTORYGUIEXTENDERSERVICE_H
