#include "cxTestGrabberSenderController.h"

//#include <cppunit/extensions/HelperMacros.h>

TestGrabberSenderController::TestGrabberSenderController(QObject* parent) : QObject(parent)
{
}

void TestGrabberSenderController::initialize(cx::GrabberSenderDirectLinkPtr grabberBridge)
{
	mGrabberBridge = grabberBridge;
	connect(mGrabberBridge.get(), SIGNAL(newImage()), this, SLOT(newImageSlot()), Qt::DirectConnection);
	connect(mGrabberBridge.get(), SIGNAL(newUSStatus()), this, SLOT(newUSStatusSlot()), Qt::DirectConnection);

}

bool TestGrabberSenderController::verify()
{
//	CPPUNIT_ASSERT(mImageReceived);
//	CPPUNIT_ASSERT(mStatusReceived)
	return mImageReceived && mStatusReceived;
}

void TestGrabberSenderController::newImageSlot()
{
//	this->addImageToQueue(mGrabberBridge->popImage());
//	std::cout << "TestGrabberSenderController::newImageSlot()" << std::endl;
	mImageReceived = true;
}

void TestGrabberSenderController::newUSStatusSlot()
{
//	this->addSonixStatusToQueue(mGrabberBridge->popUSStatus());
//	std::cout << "TestGrabberSenderController::newUSStatusSlot()" << std::endl;
	mStatusReceived = true;
}
