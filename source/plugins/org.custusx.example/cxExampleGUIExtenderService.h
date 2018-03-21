/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXEXAMPLEGUIEXTENDERSERVICE_H_
#define CXEXAMPLEGUIEXTENDERSERVICE_H_

#include "cxGUIExtenderService.h"
#include "org_custusx_example_Export.h"
class ctkPluginContext;

namespace cx
{

/**
 * Implementation of Example service.
 *
 * \ingroup org_custusx_example
 *
 * \date 2014-04-01
 * \author Christian Askeland
 */
class org_custusx_example_EXPORT ExampleGUIExtenderService : public GUIExtenderService
{
	Q_INTERFACES(cx::GUIExtenderService)
public:
	ExampleGUIExtenderService(ctkPluginContext *context);
	virtual ~ExampleGUIExtenderService() {};

	std::vector<CategorizedWidget> createWidgets() const;

private:
  ctkPluginContext* mContext;

};
typedef boost::shared_ptr<ExampleGUIExtenderService> ExampleGUIExtenderServicePtr;

} /* namespace cx */

#endif /* CXEXAMPLEGUIEXTENDERSERVICE_H_ */

