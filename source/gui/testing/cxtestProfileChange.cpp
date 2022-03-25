/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "catch.hpp"

#include "cxTestCustusXController.h"
#include "cxDataLocations.h"
#include "cxLogicManager.h"
#include <QTimer>
#include "boost/function.hpp"
#include "boost/bind.hpp"

namespace cxtest
{

namespace
{

void initTest()
{
	cx::DataLocations::setTestMode();
}

}

TEST_CASE("Profile change with running gui", "[gui][integration]")
{
	initTest();

	CustusXController custusX(NULL);
	custusX.start();

	QTimer::singleShot(200, &custusX, SLOT(changeToNewProfile()));
	QTimer::singleShot(400,   qApp, SLOT(quit()) );

	qApp->exec();
	custusX.stop();

	CHECK(true);
}


}//namespace cx

