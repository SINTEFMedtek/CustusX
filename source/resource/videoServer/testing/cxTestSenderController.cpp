#include "cxTestSenderController.h"

//#include <cppunit/extensions/HelperMacros.h>

TestSenderController::TestSenderController(QObject* parent) : QObject(parent)
{
}

void TestSenderController::initialize(cx::DirectlyLinkedSenderPtr grabberBridge)
{
	mGrabberBridge = grabberBridge;
	connect(mGrabberBridge.get(), SIGNAL(newImage()), this, SLOT(newImageSlot()), Qt::DirectConnection);
	connect(mGrabberBridge.get(), SIGNAL(newUSStatus()), this, SLOT(newUSStatusSlot()), Qt::DirectConnection);

}

bool TestSenderController::verify()
{
//	CPPUNIT_ASSERT(mImageReceived);
//	CPPUNIT_ASSERT(mStatusReceived)
	return mImageReceived && mStatusReceived;
}

void TestSenderController::newImageSlot()
{
//	this->addImageToQueue(mGrabberBridge->popImage());
//	std::cout << "TestGrabberSenderController::newImageSlot()" << std::endl;
	mImageReceived = true;
}

void TestSenderController::newUSStatusSlot()
{
//	this->addSonixStatusToQueue(mGrabberBridge->popUSStatus());
//	std::cout << "TestGrabberSenderController::newUSStatusSlot()" << std::endl;
	mStatusReceived = true;
}
