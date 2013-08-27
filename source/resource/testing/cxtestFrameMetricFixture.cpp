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

#include "cxtestFrameMetricFixture.h"

#include "sscDataManagerImpl.h"
#include "sscDummyToolManager.h"
#include "cxDataLocations.h"

namespace cxtest {

FrameMetricFixture::FrameMetricFixture()
{
	ssc::DataManagerImpl::initialize();
	ssc::ToolManager::setInstance(ssc::DummyToolManager::getInstance());

	mOriginalMetric.reset(new cx::FrameMetric("testMetric"));
	mModifiedMetric.reset(new cx::FrameMetric("testMetric2"));

}

FrameMetricFixture::~FrameMetricFixture()
{

	ssc::ToolManager::shutdown();
	ssc::DummyToolManager::shutdown();
	ssc::DataManagerImpl::shutdown();
}

bool FrameMetricFixture::createAndSetTestTransform()
{
	mTransformString = "1.1 2 3 4 5 6 7 8 9 10 11 12 0 0 0 1";
	if (!this->readTransformFromString(mTransformString))
		return false;
	mTestTransform = this->mReturnedTransform;

	mOriginalMetric->setFrame(mTestTransform);
	mModifiedMetric->setFrame(mTestTransform);
	return true;
}

bool FrameMetricFixture::readTransformFromString(QString matrixString)
{
	bool transformStringOk = false;
	mReturnedTransform = ssc::Transform3D::fromString(matrixString, &transformStringOk);
	return transformStringOk;
}

bool FrameMetricFixture::isEqualTransform(ssc::Transform3D transform)
{
	return ssc::similar(mTestTransform, transform);
}

QDomNode FrameMetricFixture::createDummyXmlNode()
{
	QDomDocument document;
	document.appendChild(document.createElement("root"));
	mXmlNode = document.firstChild();
	return mXmlNode;
}

cx::FrameMetricPtr FrameMetricFixture::createFromXml(QDomNode& xmlNode)
{
	mModifiedMetric->parseXml(xmlNode);
	return mModifiedMetric;
}

void FrameMetricFixture::setPatientRegistration()
{
	ssc::Transform3DPtr testRegistration(new ssc::Transform3D(mTestTransform));
	ssc::toolManager()->set_rMpr(testRegistration);
}

void FrameMetricFixture::changeSpaceToPatientReference()
{
	mModifiedMetric->setSpace(ssc::CoordinateSystemHelpers::getPr());
}

void FrameMetricFixture::setSpaceToOrigial()
{
	mModifiedMetric->setSpace(mOriginalMetric->getSpace());
}

//bool FrameMetricFixture::saveTransformToTestFile()
//{
//	cx::DataLocations::setTestMode();
//	mTestFile = cx::DataLocations::getTestDataPath() + "/temp/metric/TransformTestFile.txt";
//	std::cout << "Test file: " << mTestFile.toStdString() << std::endl;
//	return mMetric->appendTransformToFile(mTestFile);
//}

} //namespace cxtest
