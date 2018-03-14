/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXDICOMGUIEXTENDERSERVICE_H_
#define CXDICOMGUIEXTENDERSERVICE_H_

#include "cxGUIExtenderService.h"
#include "org_custusx_dicom_Export.h"

class ctkPluginContext;

namespace cx
{

/**
 * Implementation of Dicom service.
 *
 * \ingroup org_custusx_dicom
 *
 * \date 2014-04-01
 * \author Christian Askeland
 */
class org_custusx_dicom_EXPORT DicomGUIExtenderService : public GUIExtenderService
{
	Q_INTERFACES(cx::GUIExtenderService)
public:
	DicomGUIExtenderService(ctkPluginContext* context);
	virtual ~DicomGUIExtenderService();

	std::vector<CategorizedWidget> createWidgets() const;
	ctkPluginContext* mContext;
};
typedef boost::shared_ptr<DicomGUIExtenderService> DicomGUIExtenderServicePtr;

} /* namespace cx */

#endif /* CXDICOMGUIEXTENDERSERVICE_H_ */

