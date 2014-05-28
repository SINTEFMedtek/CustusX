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

#ifndef CXUSSIMULATORGUIEXTENDERSERVICE_H
#define CXUSSIMULATORGUIEXTENDERSERVICE_H

#include "cxGUIExtenderService.h"
#include "org_custusx_ussimulator_Export.h"

namespace cx
{

/**
 * Implementation of US simulator service.
 *
 * \ingroup org_custusx_ussimulator
 *
 * \date 2014-05-28
 * \author Ole Vegard Solberg, SINTEF
 */
class org_custusx_ussimulator_EXPORT USSimulatorGUIExtenderService : public GUIExtenderService
{
	Q_INTERFACES(cx::GUIExtenderService)
public:
	USSimulatorGUIExtenderService();
	virtual ~USSimulatorGUIExtenderService();

	std::vector<CategorizedWidget> createWidgets() const;

};
typedef boost::shared_ptr<USSimulatorGUIExtenderService> USSimulatorGUIExtenderServicePtr;

} // namespace cx

#endif // CXUSSIMULATORGUIEXTENDERSERVICE_H
