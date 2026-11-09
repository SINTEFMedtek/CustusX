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
#include <functional>
#include "boost/bind/bind.hpp"

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

	// Plugin widgets (e.g. Fraxinus' VB widget) rely on pluginsStarted after a profile change.
	int pluginsStartedCount = 0;
	QMetaObject::Connection connection = QObject::connect(cx::LogicManager::getInstance(), &cx::LogicManager::pluginsStarted,
														  [&pluginsStartedCount](){ ++pluginsStartedCount; qApp->quit(); });

	QTimer::singleShot(200, &custusX, SLOT(changeToNewProfile()));
	// Fallback if pluginsStarted never comes. Local, so it can't quit a later test's event loop.
	QTimer fallbackTimer;
	fallbackTimer.setSingleShot(true);
	QObject::connect(&fallbackTimer, &QTimer::timeout, qApp, &QCoreApplication::quit);
	fallbackTimer.start(20000);

	qApp->exec();
	QObject::disconnect(connection);
	custusX.stop();

	CHECK(pluginsStartedCount == 1);
}


}//namespace cx

