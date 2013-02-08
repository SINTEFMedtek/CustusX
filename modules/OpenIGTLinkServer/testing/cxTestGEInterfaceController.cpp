/*
 * cxTestGEInterfaceController.cpp
 *
 *  \date Feb 5, 2012
 *      \author Ole Vegard Solberg
 */

#include "cxTestGEInterfaceController.h"

#include <cppunit/extensions/HelperMacros.h>

TestGEInterfaceController::TestGEInterfaceController(QObject* parent) : QObject(parent)
{
}

void TestGEInterfaceController::initialize(cx::GrabberSenderDirectLinkPtr grabberBridge)
{
	mGrabberBridge = grabberBridge;
	connect(mGrabberBridge.get(), SIGNAL(newImage()), this, SLOT(newImageSlot()), Qt::DirectConnection);
	connect(mGrabberBridge.get(), SIGNAL(newUSStatus()), this, SLOT(newUSStatusSlot()), Qt::DirectConnection);

}


void TestGEInterfaceController::verify()
{
	CPPUNIT_ASSERT(mImageReceived);
	CPPUNIT_ASSERT(mStatusReceived);
}

void TestGEInterfaceController::newImageSlot()
{
//	this->addImageToQueue(mGrabberBridge->popImage());
//	std::cout << "TestGEInterfaceController::newImageSlot()" << std::endl;
	mImageReceived = true;
}

void TestGEInterfaceController::newUSStatusSlot()
{
//	this->addSonixStatusToQueue(mGrabberBridge->popUSStatus());
//	std::cout << "TestGEInterfaceController::newUSStatusSlot()" << std::endl;
	mStatusReceived = true;
}
