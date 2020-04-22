/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxtestTestVideoConnectionWidget.h"

#include <QtTest/QtTest>
#include <QComboBox>
#include <QPushButton>
#include "cxVideoSource.h"
#include "cxStringProperty.h"
#include "cxVideoService.h"
#include "cxtestQueuedSignalListener.h"
#include "cxtestUtilities.h"
#include "cxImage.h"
#include "cxPatientModelService.h"
#include "cxDataLocations.h"
#include "cxStreamerServiceUtilities.h"
#include "cxProfile.h"
#include "cxActiveData.h"
#include "catch.hpp"
#include "cxLogger.h"

namespace cxtest
{

TestVideoConnectionWidget::TestVideoConnectionWidget(cx::VisServicesPtr services) :
	VideoConnectionWidget(services, NULL)
{
}

bool TestVideoConnectionWidget::canStream(QString filename)
{
	CX_LOG_DEBUG() << "TestVideoConnectionWidget::canStream filename: " << filename;
	this->show();
	QTest::qWaitForWindowActive(this);
	CX_LOG_DEBUG() << "TestVideoConnectionWidget::canStream: Window active";
	this->setupWidgetToRunStreamer(filename);
	CX_LOG_DEBUG() << "TestVideoConnectionWidget::canStream: setupWidgetToRunStreamer";

	QTest::mouseClick(mConnectButton, Qt::LeftButton); //connect
	CX_LOG_DEBUG() << "TestVideoConnectionWidget::canStream: connect";

	REQUIRE(waitForQueuedSignal(mServices->video().get(), SIGNAL(connected(bool)), 1000));
	CX_LOG_DEBUG() << "TestVideoConnectionWidget::canStream: got connected signal";
	REQUIRE(waitForQueuedSignal(mServices->video().get(), SIGNAL(activeVideoSourceChanged()), 30000));
	CX_LOG_DEBUG() << "TestVideoConnectionWidget::canStream: got activeVideoSourceChanged signal";
	cx::VideoSourcePtr stream = mServices->video()->getActiveVideoSource();
	REQUIRE(waitForQueuedSignal(stream.get(), SIGNAL(newFrame()), 1000));
	CX_LOG_DEBUG() << "TestVideoConnectionWidget::canStream: got newFrame signal";
	bool canStream = stream->isStreaming();

	QTest::mouseClick(mConnectButton, Qt::LeftButton); //disconnect
	CX_LOG_DEBUG() << "TestVideoConnectionWidget::canStream: disconnect";

	this->close();

	CX_LOG_DEBUG() << "TestVideoConnectionWidget::canStream finished. canStream: " << canStream;
	return canStream;
}

cx::PropertyPtr TestVideoConnectionWidget::getOption(QString uid, QString method)
{
	cx::XmlOptionFile options = cx::profile()->getXmlSettings().descend("video");
	QDomElement element = options.getElement("video");
    cx::StreamerServicePtr streamer = mServices->video()->getStreamerService(method);
	cx::PropertyPtr option = cx::Property::findProperty(streamer->getSettings(element), uid);
	return option;
}

void TestVideoConnectionWidget::setupWidgetToRunStreamer(QString filename)
{
	cx::ActiveDataPtr activeData = mServices->patient()->getActiveData();
	cx::ImagePtr image = Utilities::create3DImage();
	mServices->patient()->insertData(image);
	activeData->setActive(image);

	QString method = "image_file_streamer";
	mConnectionSelector->setValue(method);
	this->getOption("filename", method)->setValueFromVariant(filename);
	this->getOption("runlocalserver", method)->setValueFromVariant(false);
}

} /* namespace cxtest */
