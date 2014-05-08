// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.

#ifndef CXDICOMGUIEXTENDERSERVICE_H_
#define CXDICOMGUIEXTENDERSERVICE_H_

#include "cxGUIExtenderService.h"
#include "org_custusx_dicom_Export.h"

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
	DicomGUIExtenderService();
	virtual ~DicomGUIExtenderService();

	std::vector<CategorizedWidget> createWidgets() const;

};
typedef boost::shared_ptr<DicomGUIExtenderService> DicomGUIExtenderServicePtr;

} /* namespace cx */

#endif /* CXDICOMGUIEXTENDERSERVICE_H_ */

