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

namespace cxtest
{

TestVideoConnectionWidget::TestVideoConnectionWidget(cx::VisServicesPtr services) :
	VideoConnectionWidget(services, NULL)
{
}

bool TestVideoConnectionWidget::canStream(QString filename)
{
	this->show();
	QTest::qWaitForWindowActive(this);

	this->setupWidgetToRunStreamer(filename);

	QTest::mouseClick(mConnectButton, Qt::LeftButton); //connect

	REQUIRE(waitForQueuedSignal(mServices->video().get(), SIGNAL(connected(bool)), 1000));
	REQUIRE(waitForQueuedSignal(mServices->video().get(), SIGNAL(activeVideoSourceChanged()), 30000));
	cx::VideoSourcePtr stream = mServices->video()->getActiveVideoSource();
	REQUIRE(waitForQueuedSignal(stream.get(), SIGNAL(newFrame()), 1000));
	bool canStream = stream->isStreaming();

	QTest::mouseClick(mConnectButton, Qt::LeftButton); //disconnect

	this->close();

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
