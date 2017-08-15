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

#include "catch.hpp"

#include <QFile>
#include <QString>
#include <vtkImageData.h>
#include "vtkForwardDeclarations.h"
#include "cxImage.h"
#include "cxDataLocations.h"
#include "cxMHDImageStreamer.h"

#include "cxtestJenkinsMeasurement.h"

#include "cxSettings.h"
#include "cxTypeConversions.h"
//#include "cxtestSimulatedImageStreamerFixture.h"
#include "cxtestSender.h"
#include "cxtestQueuedSignalListener.h"

namespace cxtest
{
namespace {

cx::DummyImageStreamerPtr createRunningDummyImageStreamer(TestSenderPtr& sender, bool secondaryStream = false, bool sendonce = false)
{
	QString filename = cx::DataLocations::getTestDataPath() + "/testing/default_volume/Default.mhd";
	REQUIRE(QFile::exists(filename));

	cx::DummyImageStreamerPtr imagestreamer(new cx::DummyImageStreamer());
	REQUIRE(imagestreamer);

	imagestreamer->initialize(filename, secondaryStream, sendonce);
	imagestreamer->startStreaming(sender);
	REQUIRE(imagestreamer->isStreaming());
	return imagestreamer;
}

//Similar to SimulatedImageStreamerFixture::checkSenderGotImageFromStreamer()
void checkSenderGotImageFromStreamer(TestSenderPtr& sender)
{
	REQUIRE(sender);
	cx::PackagePtr package = sender->getSentPackage();
	REQUIRE(package);
	cx::ImagePtr image = package->mImage;
	REQUIRE(image);
}

//Similar to SimulatedImageStreamerFixture::checkSimulatedFrame()
void checkSimulatedFrame(TestSenderPtr& sender)
{
	REQUIRE(sender);
	REQUIRE(waitForQueuedSignal(sender.get(), SIGNAL(newPackage()), 200, true));
	checkSenderGotImageFromStreamer(sender);
}

//Similar to SimulatedImageStreamerFixture::checkSimulatedFrames()
void checkSimulatedFrames(TestSenderPtr& sender, int numFrames)
{
	for(int i = 0; i < numFrames; ++i)
		checkSimulatedFrame(sender);
}

} //empty namespace

TEST_CASE("DummyImageStreamer: File should be read and sent only once", "[streaming][unit]")
{
	TestSenderPtr sender(new TestSender());
	bool sendImageOnce = true;
	bool sendTwoStreams = false;
	cx::StreamerPtr imagestreamer = createRunningDummyImageStreamer(sender, sendTwoStreams, sendImageOnce);

//	SimulatedImageStreamerFixture fixture(sender);
//	fixture.checkSimulatedFrames(1);
	checkSimulatedFrames(sender, 1);

	REQUIRE_FALSE(waitForQueuedSignal(sender.get(), SIGNAL(newPackage())));

	imagestreamer->stopStreaming();
}

TEST_CASE("DummyImageStreamer: File should be read and send slices with a given interval", "[streaming][unit]")
{
	TestSenderPtr sender(new TestSender());
	bool sendTwoStreams = false;
	cx::StreamerPtr imagestreamer = createRunningDummyImageStreamer(sender,sendTwoStreams);

	int numFrames = 2;
//	SimulatedImageStreamerFixture fixture(sender);
//	fixture.checkSimulatedFrames(numFrames);
	checkSimulatedFrames(sender, numFrames);

	imagestreamer->stopStreaming();
}

}//namespace cxtest
