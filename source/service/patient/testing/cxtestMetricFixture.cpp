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

#include "cxtestMetricFixture.h"

#include "sscDataManagerImpl.h"
#include "cxDataManager.h"
#include "sscDummyToolManager.h"
#include "cxDataLocations.h"
#include "catch.hpp"
#include "sscTypeConversions.h"
#include "cxStringHelpers.h"

namespace cxtest {

MetricFixture::MetricFixture()
{
	cx::cxDataManager::initialize();
	cx::ToolManager::setInstance(cx::DummyToolManager::getInstance());
}

MetricFixture::~MetricFixture()
{
	cx::ToolManager::shutdown();
	cx::DummyToolManager::shutdown();
	cx::cxDataManager::shutdown();
}


FrameMetricWithInput MetricFixture::getFrameMetricWithInput()
{
	FrameMetricWithInput retval;

	retval.m_qMt = cx::createTransformRotateZ(M_PI_2) * cx::createTransformTranslate(cx::Vector3D(1,2,3));
	retval.mSpace = cx::CoordinateSystemHelpers::getR();

	retval.mMetric = cx::FrameMetric::create("testMetric%1");
    retval.mMetric->setFrame(retval.m_qMt);
    retval.mMetric->setSpace(retval.mSpace);
	cx::cxDataManager::getInstance()->loadData(retval.mMetric);

    return retval;
}

ToolMetricWithInput MetricFixture::getToolMetricWithInput()
{
	ToolMetricWithInput retval;

	retval.m_qMt = cx::createTransformRotateZ(M_PI_2) * cx::createTransformTranslate(cx::Vector3D(1,2,3));
	retval.mSpace = cx::CoordinateSystemHelpers::getR();
	retval.mName = "TestTool";
	retval.mOffset = 5;

	retval.mMetric = cx::ToolMetric::create("testMetric%1");
	retval.mMetric->setFrame(retval.m_qMt);
	retval.mMetric->setSpace(retval.mSpace);
	retval.mMetric->setToolName(retval.mName);
	retval.mMetric->setToolOffset(retval.mOffset);
	cx::cxDataManager::getInstance()->loadData(retval.mMetric);

	return retval;
}

PointMetricWithInput MetricFixture::getPointMetricWithInput(cx::Vector3D point)
{
	PointMetricWithInput retval;

    retval.mPoint = point;
	retval.mSpace = cx::CoordinateSystemHelpers::getR();

	retval.mMetric = cx::PointMetric::create("testMetric%1");
    retval.mMetric->setCoordinate(point);
    retval.mMetric->setSpace(retval.mSpace);
	cx::cxDataManager::getInstance()->loadData(retval.mMetric);

    return retval;
}

PlaneMetricWithInput MetricFixture::getPlaneMetricWithInput(cx::Vector3D point, cx::Vector3D normal)
{
	PlaneMetricWithInput retval;

    retval.mPoint = point;
    retval.mNormal = normal;
	retval.mSpace = cx::CoordinateSystemHelpers::getR();

	retval.mMetric = cx::PlaneMetric::create("testMetric%1");
    retval.mMetric->setCoordinate(point);
    retval.mMetric->setNormal(normal);
    retval.mMetric->setSpace(retval.mSpace);
	cx::cxDataManager::getInstance()->loadData(retval.mMetric);

    return retval;
}

DistanceMetricWithInput MetricFixture::getDistanceMetricWithInput(double distance, cx::DataMetricPtr p0, cx::DataMetricPtr p1)
{
	DistanceMetricWithInput retval;

    retval.mDistance = distance;

	retval.mMetric = cx::DistanceMetric::create("testMetric%1");
	retval.mMetric->getArguments()->set(0, p0);
	retval.mMetric->getArguments()->set(1, p1);
	cx::cxDataManager::getInstance()->loadData(retval.mMetric);

    return retval;
}

DistanceMetricWithInput MetricFixture::getDistanceMetricWithInput(double distance)
{
	DistanceMetricWithInput retval;

    retval.mDistance = distance;

	retval.mMetric = cx::DistanceMetric::create("testMetric%1");
	retval.mMetric->getArguments()->set(0, this->getPointMetricWithInput(cx::Vector3D(0,0,0)).mMetric);
	retval.mMetric->getArguments()->set(1, this->getPointMetricWithInput(cx::Vector3D(distance,0,0)).mMetric);
	cx::cxDataManager::getInstance()->loadData(retval.mMetric);

    return retval;
}


bool MetricFixture::inputEqualsMetric(FrameMetricWithInput data)
{
    INFO("qMt");
    INFO(qstring_cast(data.m_qMt));
    INFO("==");
    INFO(qstring_cast(data.mMetric->getFrame()));
    //    INFO("Space: " + qstring_cast(data.mSpace));

	return cx::similar(data.m_qMt, data.mMetric->getFrame())
            && (data.mSpace == data.mMetric->getSpace());
}

bool MetricFixture::inputEqualsMetric(ToolMetricWithInput data)
{
	INFO("qMt");
	INFO(qstring_cast(data.m_qMt));
	INFO("==");
	INFO(qstring_cast(data.mMetric->getFrame()));
	//    INFO("Space: " + qstring_cast(data.mSpace));

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

	return (cx::similar(data.mDistance, data.mMetric->getDistance()));
}

bool MetricFixture::inputEqualsMetric(PointMetricWithInput data)
{
	return (cx::similar(data.mPoint, data.mMetric->getCoordinate()))
            && (data.mSpace == data.mMetric->getSpace());
}

bool MetricFixture::inputEqualsMetric(PlaneMetricWithInput data)
{
	return (cx::similar(data.mPoint, data.mMetric->getCoordinate()))
			&& (cx::similar(data.mNormal, data.mMetric->getNormal()))
            && (data.mSpace == data.mMetric->getSpace());
}

QStringList MetricFixture::getSingleLineDataList(cx::DataMetricPtr metric)
{
	QString singleLine = metric->getAsSingleLineString();
	CHECK(!singleLine.isEmpty());
	INFO("line: " + singleLine);
	QStringList list = cx::splitStringContaingQuotes(singleLine);
	INFO("list: " + list.join("\n"));
	CHECK(!list.empty());
	return list;
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
	cx::dataManager()->set_rMpr(testRegistration);
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

} //namespace cxtest
