/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXACQUISITIONPLUGIN_H_
#define CXACQUISITIONPLUGIN_H_

#include "org_custusx_acquisition_Export.h"

#include "cxGUIExtenderService.h"
class QDomNode;
class ctkPluginContext;

namespace cx
{
typedef boost::shared_ptr<class AcquisitionData> AcquisitionDataPtr;
typedef boost::shared_ptr<class AcquisitionPlugin> AcquisitionPluginPtr;
typedef boost::shared_ptr<class UsReconstructionService> UsReconstructionServicePtr;
typedef boost::shared_ptr<class AcquisitionService> AcquisitionServicePtr;
typedef boost::shared_ptr<class VisServices> VisServicesPtr;


/**
* \file
* \addtogroup org_custusx_acquisition
* @{
*/

/**
 *
 */
class org_custusx_acquisition_EXPORT  AcquisitionPlugin: public GUIExtenderService
{
Q_OBJECT
public:
	AcquisitionPlugin(ctkPluginContext *context);
	virtual ~AcquisitionPlugin();

	virtual std::vector<CategorizedWidget> createWidgets() const;

private:
	UsReconstructionServicePtr mUsReconstructionService;
	AcquisitionServicePtr mAcquisitionService;
	VisServicesPtr mServices;
};

/**
* @}
*/
}

#endif /* CXACQUISITIONPLUGIN_H_ */
