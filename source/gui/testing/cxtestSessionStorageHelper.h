/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXTESTSESSIONSTORAGEHELPER_H
#define CXTESTSESSIONSTORAGEHELPER_H

#include "cxForwardDeclarations.h"
#include "cxtestSessionStorageTestFixture.h"

namespace cxtest
{

/**\brief Helper class providing a simple patient with data. Used by tests.
 *
 *  \date Oct, 2015
 *  \author Ole Vegard Solberg, SINTEF
 */
class SessionStorageHelper
{
public:
	void createTestPatient();
	void createTestPatientWithData();
	cx::VisServicesPtr getServices();

	SessionStorageTestFixture storageFixture;
	TestDataStructures testData;
};

}//cxtest

#endif // CXTESTSESSIONSTORAGEHELPER_H
