/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
	REQUIRE(waitForQueuedSignal(mServices->video().get(), SIGNAL(activeVideoSourceChanged()), 2000));
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
