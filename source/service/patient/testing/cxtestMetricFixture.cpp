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
	cx::DataManager::initialize();
	ssc::ToolManager::setInstance(ssc::DummyToolManager::getInstance());
}

MetricFixture::~MetricFixture()
{
	ssc::ToolManager::shutdown();
	ssc::DummyToolManager::shutdown();
	cx::DataManager::shutdown();
}


FrameMetricData MetricFixture::getFrameMetricData()
{
    FrameMetricData retval;

    retval.m_qMt = ssc::createTransformRotateZ(M_PI_2) * ssc::createTransformTranslate(ssc::Vector3D(1,2,3));
    retval.mSpace = ssc::SpaceHelpers::getR();

	retval.mMetric = cx::FrameMetric::create("testMetric%1");
    retval.mMetric->setFrame(retval.m_qMt);
    retval.mMetric->setSpace(retval.mSpace);
	cx::DataManager::getInstance()->loadData(retval.mMetric);

    return retval;
}

ToolMetricData MetricFixture::getToolMetricData()
{
	ToolMetricData retval;

	retval.m_qMt = ssc::createTransformRotateZ(M_PI_2) * ssc::createTransformTranslate(ssc::Vector3D(1,2,3));
	retval.mSpace = ssc::SpaceHelpers::getR();
	retval.mName = "TestTool";
	retval.mOffset = 5;

	retval.mMetric = cx::ToolMetric::create("testMetric%1");
	retval.mMetric->setFrame(retval.m_qMt);
	retval.mMetric->setSpace(retval.mSpace);
	retval.mMetric->setToolName(retval.mName);
	retval.mMetric->setToolOffset(retval.mOffset);
	cx::DataManager::getInstance()->loadData(retval.mMetric);

	return retval;
}

PointMetricData MetricFixture::getPointMetricData(ssc::Vector3D point)
{
    PointMetricData retval;

    retval.mPoint = point;
    retval.mSpace = ssc::SpaceHelpers::getR();

	retval.mMetric = ssc::PointMetric::create("testMetric%1");
    retval.mMetric->setCoordinate(point);
    retval.mMetric->setSpace(retval.mSpace);
    cx::DataManager::getInstance()->loadData(retval.mMetric);

    return retval;
}

PlaneMetricData MetricFixture::getPlaneMetricData(ssc::Vector3D point, ssc::Vector3D normal)
{
    PlaneMetricData retval;

    retval.mPoint = point;
    retval.mNormal = normal;
    retval.mSpace = ssc::SpaceHelpers::getR();

	retval.mMetric = ssc::PlaneMetric::create("testMetric%1");
    retval.mMetric->setCoordinate(point);
    retval.mMetric->setNormal(normal);
    retval.mMetric->setSpace(retval.mSpace);
	cx::DataManager::getInstance()->loadData(retval.mMetric);

    return retval;
}

DistanceMetricData MetricFixture::getDistanceMetricData(double distance, ssc::DataMetricPtr p0, ssc::DataMetricPtr p1)
{
    DistanceMetricData retval;

    retval.mDistance = distance;

	retval.mMetric = ssc::DistanceMetric::create("testMetric%1");
    retval.mMetric->setArgument(0, p0);
    retval.mMetric->setArgument(1, p1);
	cx::DataManager::getInstance()->loadData(retval.mMetric);

    return retval;
}

DistanceMetricData MetricFixture::getDistanceMetricData(double distance)
{
    DistanceMetricData retval;

    retval.mDistance = distance;

	retval.mMetric = ssc::DistanceMetric::create("testMetric%1");
    retval.mMetric->setArgument(0, this->getPointMetricData(ssc::Vector3D(0,0,0)).mMetric);
    retval.mMetric->setArgument(1, this->getPointMetricData(ssc::Vector3D(distance,0,0)).mMetric);
	cx::DataManager::getInstance()->loadData(retval.mMetric);

    return retval;
}


bool MetricFixture::metricEqualsData(FrameMetricData data)
{
    INFO("qMt");
    INFO(qstring_cast(data.m_qMt));
    INFO("==");
    INFO(qstring_cast(data.mMetric->getFrame()));
    //    INFO("Space: " + qstring_cast(data.mSpace));

    return ssc::similar(data.m_qMt, data.mMetric->getFrame())
            && (data.mSpace == data.mMetric->getSpace());
}

bool MetricFixture::metricEqualsData(ToolMetricData data)
{
	INFO("qMt");
	INFO(qstring_cast(data.m_qMt));
	INFO("==");
	INFO(qstring_cast(data.mMetric->getFrame()));
	//    INFO("Space: " + qstring_cast(data.mSpace));

	return ssc::similar(data.m_qMt, data.mMetric->getFrame())
			&& (data.mSpace == data.mMetric->getSpace())
			&& (ssc::similar(data.mOffset, data.mMetric->getToolOffset()))
			&& (data.mName == data.mMetric->getToolName());
}

bool MetricFixture::metricEqualsData(DistanceMetricData data)
{
    INFO("distance");
    INFO(qstring_cast(data.mDistance));
    INFO("==");
    INFO(qstring_cast(data.mMetric->getDistance()));

    INFO("p0:");
    if (data.mMetric->getArgument(0))
        INFO(streamXml2String(*data.mMetric->getArgument(0)));
    INFO("p1:");
    if (data.mMetric->getArgument(1))
        INFO(streamXml2String(*data.mMetric->getArgument(1)));

    return (ssc::similar(data.mDistance, data.mMetric->getDistance()));
}

bool MetricFixture::metricEqualsData(PointMetricData data)
{
    return (ssc::similar(data.mPoint, data.mMetric->getCoordinate()))
            && (data.mSpace == data.mMetric->getSpace());
}

bool MetricFixture::metricEqualsData(PlaneMetricData data)
{
    return (ssc::similar(data.mPoint, data.mMetric->getCoordinate()))
            && (ssc::similar(data.mNormal, data.mMetric->getNormal()))
            && (data.mSpace == data.mMetric->getSpace());
}

QStringList MetricFixture::getSingleLineDataList(ssc::DataMetricPtr metric)
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
    ssc::Transform3DPtr testRegistration(new ssc::Transform3D(ssc::createTransformTranslate(ssc::Vector3D(5,6,7))));
	ssc::toolManager()->set_rMpr(testRegistration);
}

bool MetricFixture::verifySingleLineHeader(QStringList list, ssc::DataMetricPtr metric)
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
