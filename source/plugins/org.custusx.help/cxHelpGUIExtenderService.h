/*
 * cxHelpGUIExtenderService.h
 *
 *  Created on: Sep 11, 2014
 *      Author: christiana
 */

#ifndef CXHELPGUIEXTENDERSERVICE_H_
#define CXHELPGUIEXTENDERSERVICE_H_

#include "cxGUIExtenderService.h"
#include "org_custusx_help_Export.h"
class ctkPluginContext;

namespace cx
{

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
	HelpGUIExtenderService(ctkPluginContext *context);
	virtual ~HelpGUIExtenderService();

	std::vector<CategorizedWidget> createWidgets() const;

private:
  ctkPluginContext* mContext;

};
typedef boost::shared_ptr<HelpGUIExtenderService> HelpGUIExtenderServicePtr;

} /* namespace cx */

#endif /* CXHELPGUIEXTENDERSERVICE_H_ */
