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
	QString transformString("1.1 2 3 4  5 6 7 8  9 10 11 12  0 0 0 1");
	bool transformStringOk = false;
	mTestTransform = ssc::Transform3D::fromString(transformString, &transformStringOk);
	if(!transformStringOk)
		return false;
	mOriginalMetric->setFrame(mTestTransform);
	mModifiedMetric->setFrame(mTestTransform);
	return true;
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

QString FrameMetricFixture::expectedStringAfterConversion()
{
	return QString("frameMetric testMetric reference  1.1 2 3 4 5 6 7 8 9 10 11 12 0 0 0 1");
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
