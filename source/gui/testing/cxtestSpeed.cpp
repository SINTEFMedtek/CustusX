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

#include "catch.hpp"

#include <QTimer>

#include "sscTypeConversions.h"
#include "sscImage.h"
#include "cxtestSender.h"
#include "cxtestQueuedSignalListener.h"
#include "cxtestUtilities.h"
#include "cxTestCustusXController.h"
#include "cxDataLocations.h"
#include "cxSettings.h"


namespace cxtest
{

class JenkinsMeasurement
{
public:
	void initialize()
	{
		cout << "CTEST_FULL_OUTPUT" << endl;
	}

	// create output in a way friendly to the Jenkins measurement plugin:
	void createOutput(QString name, QString value)
	{
		QString measurement("<measurement><name>%1</name><value>%2</value></measurement>");
		measurement = measurement.arg(name).arg(value);
		std::cout << measurement << std::endl;
	}

};

TEST_CASE("Speed: Run CustusX with a minimum render speed", "[speed][gui][integration]")
{
	JenkinsMeasurement jenkins;
	jenkins.initialize();
//	cout << "CTEST_FULL_OUTPUT" << endl;
	// this stuff will be performed just before all tests in this class
	cx::DataLocations::setTestMode();
	cx::settings()->setValue("renderingInterval", 4);

	CustusXController custusX(NULL);
	custusX.mPatientFolder = cx::DataLocations::getTestDataPath() + "/Phantoms/Kaisa/CustusX/Speed_Test_Kaisa.cx3";

	custusX.start();
	qApp->exec();
	custusX.stop();

	// output FPS in a way friendly to the Jenkins measurement plugin:
//	QString measurement = defineJenkinsMeasurememt("FPS", QString::number(custusX.mMeasuredFPS));
//	std::cout << measurement << std::endl;
	jenkins.createOutput("FPS", QString::number(custusX.mMeasuredFPS));

	// TODO: enter this value into config file
	double minimumFPS = 5;
	REQUIRE(custusX.mMeasuredFPS > minimumFPS);
}

TEST_CASE("Speed: Run CustusX with interactive slicing at a minimum render speed", "[speed][gui][integration]")
{
	JenkinsMeasurement jenkins;
	jenkins.initialize();

	// this stuff will be performed just before all tests in this class
	cx::DataLocations::setTestMode();
	cx::settings()->setValue("renderingInterval", 4);

	CustusXController custusX(NULL);
	custusX.mPatientFolder = cx::DataLocations::getTestDataPath() + "/Phantoms/Kaisa/CustusX/Speed_Test_Kaisa.cx3";
	custusX.mEnableSlicing = true;

	custusX.start();
	qApp->exec();
	custusX.stop();

	jenkins.createOutput("FPS_Slicing", QString::number(custusX.mMeasuredFPS));

	// TODO: enter this value into config file
	double minimumFPS = 5;
	REQUIRE(custusX.mMeasuredFPS > minimumFPS);
}

}//namespace cx
