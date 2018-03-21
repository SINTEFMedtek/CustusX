/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXVBSERVICE_H_
#define CXVBSERVICE_H_

#include "cxGUIExtenderService.h"
#include "org_custusx_virtualbronchoscopy_Export.h"
class ctkPluginContext;

namespace cx
{
typedef boost::shared_ptr<class VisServices> VisServicesPtr;

/**
 * Virtual Bronchoscopy service interface.
 *
 * \ingroup org_custusx_virtualbronchoscopy
 *
 * \date Aug 26, 2015
 * \author Geir Arne Tangen, SINTEF
 */
class org_custusx_virtualbronchoscopy_EXPORT VBGUIExtenderService : public GUIExtenderService
{
	Q_INTERFACES(cx::GUIExtenderService)
public:
	VBGUIExtenderService(VisServicesPtr services);
	virtual ~VBGUIExtenderService() {}
	std::vector<CategorizedWidget> createWidgets() const;

//	virtual QString getWidgetName() {return QString("org_custusx_virtualbronchoscopy_widget");}
private:
	VisServicesPtr mServices;
};
typedef boost::shared_ptr<VBGUIExtenderService> VBGUIExtenderServicePtr;

} /* namespace cx */

#endif /* CXM2USSERVICE_H_ */

