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

#ifndef CXEXAMPLEGUIEXTENDERSERVICE_H_
#define CXEXAMPLEGUIEXTENDERSERVICE_H_

#include "cxGUIExtenderService.h"
#include "org_custusx_example_Export.h"

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
	ExampleGUIExtenderService();
	virtual ~ExampleGUIExtenderService();

	std::vector<CategorizedWidget> createWidgets() const;

};
typedef boost::shared_ptr<ExampleGUIExtenderService> ExampleGUIExtenderServicePtr;

} /* namespace cx */

#endif /* CXEXAMPLEGUIEXTENDERSERVICE_H_ */

