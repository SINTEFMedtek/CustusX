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

namespace cxtest {

MetricFixture::MetricFixture()
{
    cx::DataManager::initialize();
	ssc::ToolManager::setInstance(ssc::DummyToolManager::getInstance());

//	mOriginalMetric.reset(new cx::FrameMetric("testMetric"));
//	mModifiedMetric.reset(new cx::FrameMetric("testMetric2"));
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






//bool MetricFixture::createAndSetTestTransform()
//{
//	mTransformString = "1.1 2 3 4 5 6 7 8 9 10 11 12 0 0 0 1";
//	if (!this->readTransformFromString(mTransformString))
//		return false;
//	mTestTransform = this->mReturnedTransform;

//	mOriginalMetric->setFrame(mTestTransform);
//	mModifiedMetric->setFrame(mTestTransform);
//	return true;
//}

//bool MetricFixture::readTransformFromString(QString matrixString)
//{
//	bool transformStringOk = false;
//	mReturnedTransform = ssc::Transform3D::fromString(matrixString, &transformStringOk);
//	return transformStringOk;
//}

//bool MetricFixture::isEqualTransform(ssc::Transform3D transform)
//{
//	return ssc::similar(mTestTransform, transform);
//}

QDomNode MetricFixture::createDummyXmlNode()
{
	QDomDocument document;
	document.appendChild(document.createElement("root"));
    return document.firstChild();
}

//cx::FrameMetricPtr MetricFixture::createFromXml(QDomNode& xmlNode)
//{
//	mModifiedMetric->parseXml(xmlNode);
//	return mModifiedMetric;
//}

void MetricFixture::setPatientRegistration()
{
    ssc::Transform3DPtr testRegistration(new ssc::Transform3D(ssc::createTransformTranslate(ssc::Vector3D(5,6,7))));
	ssc::toolManager()->set_rMpr(testRegistration);
}

//void MetricFixture::changeSpaceToPatientReference()
//{
//	mModifiedMetric->setSpace(ssc::CoordinateSystemHelpers::getPr());
//}

//void MetricFixture::setSpaceToOrigial()
//{
//	mModifiedMetric->setSpace(mOriginalMetric->getSpace());
//}

////bool MetricFixture::saveTransformToTestFile()
////{
////	cx::DataLocations::setTestMode();
////	mTestFile = cx::DataLocations::getTestDataPath() + "/temp/metric/TransformTestFile.txt";
////	std::cout << "Test file: " << mTestFile.toStdString() << std::endl;
////	return mMetric->appendTransformToFile(mTestFile);
////}

/** given a text line containing fex [alpha beta "gamm ma" delta]
  * split into a list of ["alpha", "beta" ,"gamm ma", "delta"]
  */
QStringList MetricFixture::splitStringLineIntoTextComponents(QString line)
{
    QStringList base = line.split(" ");
    QStringList retval;

    for (int i=0; i<base.size(); ++i)
    {
        if (base[i].startsWith("\""))
        {
            retval.push_back("");
            for (; i<base.size() && !base[i].endsWith("\""); ++i)
                retval.back() += base[i];
            if (!i<base.size())
                retval.back() += base[i];
        }
        else
        {
            retval.push_back(base[i]);
        }

        retval.back() = retval.back().remove("\"");
    }

    return retval;
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
