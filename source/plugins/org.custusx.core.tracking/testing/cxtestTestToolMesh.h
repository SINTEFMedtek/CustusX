/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXTESTTESTTOOLMESH_H_
#define CXTESTTESTTOOLMESH_H_

#include "cxtest_org_custusx_core_tracking_export.h"

#include "cxMesh.h"
#include <QString>

namespace cxtest {

/*
 * TestToolMesh
 * \brief Helper class for testing tool mesh file
 * \date Aug 8, 2013
 * \author Ole Vegard Solberg, SINTEF
 */
class CXTEST_ORG_CUSTUSX_CORE_TRACKING_EXPORT  TestToolMesh {
public:
	TestToolMesh();
	void setToolPath(QString path);
	bool canLoadMesh(QString filename);
private:
	QString mToolToolPath;
	QString mCurrentToolPath;
};

} /* namespace cxtest */
#endif /* CXTESTTESTTOOLMESH_H_ */
