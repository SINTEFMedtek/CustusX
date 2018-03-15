/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxtestMetricFixture.h"

#include "cxDummyToolManager.h"
#include "cxDataLocations.h"
#include "catch.hpp"
#include "cxTypeConversions.h"
#include "cxStringHelpers.h"
#include "cxSpaceProviderImpl.h"
#include "cxRegistrationTransform.h"
#include "cxLogicManager.h"


namespace cxtest {

MetricFixture::MetricFixture()
{
	mMessageListener = cx::MessageListener::createWithQueue();
	mServices = TestVisServices::create();
}

MetricFixture::~MetricFixture()
{
	mServices.reset();
	CHECK(!mMessageListener->containsErrors());
}

cx::SpaceProviderPtr MetricFixture::getSpaceProvider()
{
	return mServices->spaceProvider();
}

FrameMetricWithInput MetricFixture::getFrameMetricWithInput()
{
	FrameMetricWithInput retval;

	retval.m_qMt = cx::createTransformRotateZ(M_PI_2) * cx::createTransformTranslate(cx::Vector3D(1,2,3));
	retval.mSpace = cx::CoordinateSystem::reference();

	retval.mMetric = this->createTestMetric<cx::FrameMetric>("testMetric%1");
    retval.mMetric->setFrame(retval.m_qMt);
    retval.mMetric->setSpace(retval.mSpace);
	this->insertData(retval.mMetric);

    return retval;
}

ToolMetricWithInput MetricFixture::getToolMetricWithInput()
{
	ToolMetricWithInput retval;

	retval.m_qMt = cx::createTransformRotateZ(M_PI_2) * cx::createTransformTranslate(cx::Vector3D(1,2,3));
	retval.mSpace = cx::CoordinateSystem::reference();
	retval.mName = "TestTool";
	retval.mOffset = 5;

	retval.mMetric = this->createTestMetric<cx::ToolMetric>("testMetric%1");
	retval.mMetric->setFrame(retval.m_qMt);
	retval.mMetric->setSpace(retval.mSpace);
	retval.mMetric->setToolName(retval.mName);
	retval.mMetric->setToolOffset(retval.mOffset);
	this->insertData(retval.mMetric);

	return retval;
}

PointMetricWithInput MetricFixture::getPointMetricWithInput(cx::Vector3D point)
{
	PointMetricWithInput retval;

    retval.mPoint = point;
	retval.mSpace = cx::CoordinateSystem::reference();

	retval.mMetric = this->createTestMetric<cx::PointMetric>("testMetric%1");
	retval.mMetric->setCoordinate(point);
    retval.mMetric->setSpace(retval.mSpace);
	this->insertData(retval.mMetric);

    return retval;
}

PlaneMetricWithInput MetricFixture::getPlaneMetricWithInput(cx::Vector3D point, cx::Vector3D normal, cx::DataMetricPtr p0, cx::DataMetricPtr p1)
{
	PlaneMetricWithInput retval;

	retval.mPoint = point;
	retval.mNormal = normal;

	retval.mMetric = this->createTestMetric<cx::PlaneMetric>("testMetric%1");
	retval.mMetric->getArguments()->set(0, p0);
	retval.mMetric->getArguments()->set(1, p1);
	this->insertData(retval.mMetric);

    return retval;
}

DistanceMetricWithInput MetricFixture::getDistanceMetricWithInput(double distance, cx::DataMetricPtr p0, cx::DataMetricPtr p1)
{
	DistanceMetricWithInput retval;

    retval.mDistance = distance;

	retval.mMetric = this->createTestMetric<cx::DistanceMetric>("testMetric%1");
	retval.mMetric->getArguments()->set(0, p0);
	retval.mMetric->getArguments()->set(1, p1);
	this->insertData(retval.mMetric);

    return retval;
}

DistanceMetricWithInput MetricFixture::getDistanceMetricWithInput(double distance)
{
	DistanceMetricWithInput retval;

    retval.mDistance = distance;

	retval.mMetric = this->createTestMetric<cx::DistanceMetric>("testMetric%1");
	retval.mMetric->getArguments()->set(0, this->getPointMetricWithInput(cx::Vector3D(0,0,0)).mMetric);
	retval.mMetric->getArguments()->set(1, this->getPointMetricWithInput(cx::Vector3D(distance,0,0)).mMetric);
	this->insertData(retval.mMetric);

    return retval;
}


bool MetricFixture::inputEqualsMetric(FrameMetricWithInput data)
{
    INFO("qMt");
    INFO(qstring_cast(data.m_qMt));
    INFO("==");
    INFO(qstring_cast(data.mMetric->getFrame()));

	return cx::similar(data.m_qMt, data.mMetric->getFrame())
            && (data.mSpace == data.mMetric->getSpace());
}

bool MetricFixture::inputEqualsMetric(ToolMetricWithInput data)
{
	INFO("qMt");
	INFO(qstring_cast(data.m_qMt));
	INFO("==");
	INFO(qstring_cast(data.mMetric->getFrame()));

	return cx::similar(data.m_qMt, data.mMetric->getFrame())
			&& (data.mSpace == data.mMetric->getSpace())
			&& (cx::similar(data.mOffset, data.mMetric->getToolOffset()))
			&& (data.mName == data.mMetric->getToolName());
}

bool MetricFixture::inputEqualsMetric(DistanceMetricWithInput data)
{
    INFO("distance");
    INFO(qstring_cast(data.mDistance));
    INFO("==");
    INFO(qstring_cast(data.mMetric->getDistance()));

    INFO("p0:");
	if (data.mMetric->getArguments()->get(0))
		INFO(streamXml2String(*data.mMetric->getArguments()->get(0)));
    INFO("p1:");
	if (data.mMetric->getArguments()->get(1))
		INFO(streamXml2String(*data.mMetric->getArguments()->get(1)));

	CHECK(cx::similar(data.mDistance, data.mMetric->getDistance()));
	return (cx::similar(data.mDistance, data.mMetric->getDistance()));
}

bool MetricFixture::inputEqualsMetric(PointMetricWithInput data)
{
	return (cx::similar(data.mPoint, data.mMetric->getCoordinate()))
            && (data.mSpace == data.mMetric->getSpace());
}

bool MetricFixture::inputEqualsMetric(PlaneMetricWithInput data)
{
	return (cx::similar(data.mPoint, data.mMetric->getRefCoord()))
			&& (cx::similar(data.mNormal, data.mMetric->getRefNormal()));
}

QDomNode MetricFixture::createDummyXmlNode()
{
	QDomDocument document;
	document.appendChild(document.createElement("root"));
    return document.firstChild();
}

void MetricFixture::setPatientRegistration()
{
	cx::Transform3D testRegistration;
	testRegistration = cx::Transform3D(cx::createTransformTranslate(cx::Vector3D(5,6,7)));
	mServices->patient()->get_rMpr_History()->setRegistration(testRegistration);
}

void MetricFixture::insertData(cx::DataPtr data)
{
	mServices->patient()->insertData(data);
}

bool MetricFixture::verifySingleLineHeader(QStringList list, cx::DataMetricPtr metric)
{
    if (list.size()<2)
        return false;
    if (list[0]!=metric->getType())
        return false;
    if (list[1]!=metric->getName())
        return false;
    return true;
}

void MetricFixture::testExportAndImportMetrics()
{
	cx::DataLocations::setTestMode();
	cx::MetricManager manager(cx::logicManager()->getViewService(), cx::logicManager()->getPatientModelService(), cx::logicManager()->getTrackingService(), cx::logicManager()->getSpaceProvider());

	// create metrics and insert them into the patientmodel
	std::vector<cx::DataMetricPtr> metrics = this->createMetricsForExport();

	// export metrics
	QString metricsFilePath = cx::DataLocations::getTestDataPath() + "/testing/metrics_export_import/exported_and_imported_metrics_file.xml";
	if(QFile::exists(metricsFilePath))
		QFile::remove(metricsFilePath);

	manager.exportMetricsToXMLFile(metricsFilePath);

	// remove the metrics from the patientmodel
	foreach (cx::DataMetricPtr metric, metrics)
	{
		cx::logicManager()->getPatientModelService()->removeData(metric->getUid());
	}

	// import the exported metrics into the patientmodel
	manager.importMetricsFromXMLFile(metricsFilePath);

	//get imported metrics from the patient and check that they are equal to the ones which was exported
	this->checkImportedMetricsEqualToExported(metrics, manager);
}

std::vector<cx::DataMetricPtr> MetricFixture::createMetricsForExport()
{
	std::vector<cx::DataMetricPtr> metrics;
	cx::Vector3D pos(0,0,0);
	cx::Vector3D pos2(1,0,0);
	cx::CoordinateSystem cs(cx::csPATIENTREF);

	//Create one of each metric to test export and import on
	cx::PointMetricPtr point = getPointMetricWithInput(pos).mMetric;
	cx::PointMetricPtr point2 = getPointMetricWithInput(pos2).mMetric;
	point->setSpace(cs);
	metrics.push_back(point);
	metrics.push_back(point2);
	metrics.push_back(getToolMetricWithInput().mMetric);
	metrics.push_back(getFrameMetricWithInput().mMetric);
	metrics.push_back(getDistanceMetricWithInput(0, point, point2).mMetric);
	metrics.push_back(getPlaneMetricWithInput(pos, pos, point, point2).mMetric);
	//The following metrics are not tested here as they had no implementation in this test fixture at the time.
	//They should ideally be tested elsewhere and this functionality should be tested ok already.
	//angle
	//sphere
	//donut
	//custom
	//roi

	//Must explicitly insert the metrics in the patient model, since the metricfixture might only have a dummy patientmodelservice.
	//The logicManager must have been initialised first.
	foreach (cx::DataMetricPtr metric, metrics)
	{
		cx::logicManager()->getPatientModelService()->insertData(metric);
	}

	return metrics;
}

void MetricFixture::checkImportedMetricsEqualToExported(std::vector<cx::DataMetricPtr>& origMetrics, cx::MetricManager& manager) const
{
	foreach (cx::DataMetricPtr metric, origMetrics)
	{
		cx::DataMetricPtr importedMetric = manager.getMetric(metric->getUid());

		REQUIRE(importedMetric);
		CHECK(metric != importedMetric); //don't compare the original metric to itself
		CHECK(metric->isEqual(importedMetric));
	}
}

} //namespace cxtest
