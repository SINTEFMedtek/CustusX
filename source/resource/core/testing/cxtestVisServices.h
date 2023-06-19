/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXTESTDUMMYDATAMANAGER_H
#define CXTESTDUMMYDATAMANAGER_H

#include "cxtestresource_export.h"

#include <QStringList>
#include "cxForwardDeclarations.h"
#include "cxVisServices.h"
#include "cxViewServiceNull.h"
class ctkPluginContext;

namespace cxtest
{

typedef boost::shared_ptr<class TestVisServices> TestVisServicesPtr;

/**
 * Test version of cx::VisServices.
 *
 * \date Oct 6 2015
 * \author Ole Vegard Solberg, SINTEF
 */
class CXTESTRESOURCE_EXPORT TestVisServices : public cx::VisServices
{
public:
	static TestVisServicesPtr create();
	TestVisServices();
	~TestVisServices();
};

} // namespace cxtest

#endif // CXTESTDUMMYDATAMANAGER_H
