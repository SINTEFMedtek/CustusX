/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXHELPGUIEXTENDERSERVICE_H_
#define CXHELPGUIEXTENDERSERVICE_H_

#include "cxGUIExtenderService.h"
#include "org_custusx_help_Export.h"
class ctkPluginContext;

namespace cx
{
typedef boost::shared_ptr<class HelpEngine> HelpEnginePtr;

/**
 * Implementation of Help service.
 *
 * \ingroup org_custusx_help
 *
 * \date 2014-09-11
 * \author Christian Askeland
 */
class org_custusx_help_EXPORT HelpGUIExtenderService : public GUIExtenderService
{
	Q_INTERFACES(cx::GUIExtenderService)
public:
	HelpGUIExtenderService(ctkPluginContext *context, HelpEnginePtr engine);
	virtual ~HelpGUIExtenderService();

	virtual std::vector<CategorizedWidget> createWidgets() const;

private:
  ctkPluginContext* mContext;
  HelpEnginePtr mEngine;
};
typedef boost::shared_ptr<HelpGUIExtenderService> HelpGUIExtenderServicePtr;

} /* namespace cx */

#endif /* CXHELPGUIEXTENDERSERVICE_H_ */
