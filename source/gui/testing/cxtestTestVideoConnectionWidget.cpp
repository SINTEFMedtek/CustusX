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
#include "cxStringDataAdapterXml.h"
#include "cxVideoServiceOld.h"
#include "cxVideoConnectionManager.h"
#include "cxtestQueuedSignalListener.h"
#include "cxtestUtilities.h"
#include "cxImage.h"
#include "cxPatientModelService.h"

//TODO: remove
#include "cxLegacySingletons.h"

namespace cxtest
{

TestVideoConnectionWidget::TestVideoConnectionWidget(cx::VisualizationServicePtr visualizationService, cx::PatientModelServicePtr patientModelService, cx::VideoServicePtr videoService) :
	VideoConnectionWidget(visualizationService, patientModelService, videoService, NULL)
{
}

bool TestVideoConnectionWidget::canStream(QString filename, QString streamerType)
{
	this->show();
	QTest::qWaitForWindowActive(this);

	this->setupWidgetToRunStreamer(filename, streamerType);

	QTest::mouseClick(mConnectButton, Qt::LeftButton); //connect

	waitForQueuedSignal(this->getVideoConnectionManager().get(), SIGNAL(connected(bool)), 1000);
	waitForQueuedSignal(cx::videoService().get(), SIGNAL(activeVideoSourceChanged()), 500);
	cx::VideoSourcePtr stream = cx::videoService()->getActiveVideoSource();
	waitForQueuedSignal(stream.get(), SIGNAL(newFrame()), 500);
	bool canStream = stream->isStreaming();

	QTest::mouseClick(mConnectButton, Qt::LeftButton); //disconnect

	this->close();

	return canStream;
}

void TestVideoConnectionWidget::setupWidgetToRunStreamer(QString filename, QString streamerType)
{
	cx::ImagePtr image = Utilities::create3DImage();
	this->mPatientModelService->setActiveImage(image);
	this->mPatientModelService->loadData(image);

	QString connectionMethod("Direct Link");
	mConnectionSelector->setValue(connectionMethod);
	QString connectionArguments("--type "+streamerType+" --filename " + filename);
	mDirectLinkArguments->addItem(connectionArguments);
	mDirectLinkArguments->setCurrentIndex(mDirectLinkArguments->findText(connectionArguments));
}

} /* namespace cxtest */
