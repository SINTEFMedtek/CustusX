#include "catch.hpp"

#include <QFile>
#include <QString>
#include "sscImage.h"
#include "cxDataLocations.h"
#include "cxMHDImageStreamer.h"
#include "cxtestSender.h"
#include "cxtestSignalListener.h"

namespace cxtest
{

cx::ImageStreamerPtr createRunningImageStreamer(TestSenderPtr& sender, bool sendonce = false)
{
	QString filename = cx::DataLocations::getTestDataPath() + "/testing/TubeSegmentationFramework/Default.mhd";
	REQUIRE(QFile::exists(filename));

	cx::StringMap args;
	args["filename"] = filename;
	args["type"] = "MHDFile";
	args["sendonce"] = sendonce ? "true" : "false";

	cx::ImageStreamerPtr imagestreamer(new cx::MHDImageStreamer());
	REQUIRE(imagestreamer);
	imagestreamer->initialize(args);
	REQUIRE(imagestreamer->startStreaming(sender));
	return imagestreamer;
}

void checkSenderGotImageFromStreamer(TestSenderPtr sender)
{
	cx::PackagePtr package = sender->getSentPackage();
	REQUIRE(package);
	ssc::ImagePtr image = package->mImage;
	REQUIRE(image);
}

TEST_CASE("MHDImageStreamer: File should be read and sent only once", "[video][unit]")
{
	TestSenderPtr sender(new TestSender());
	cx::ImageStreamerPtr imagestreamer = createRunningImageStreamer(sender, true);

	CHECK(waitForSignal(sender.get(), SIGNAL(newPackage())));
	checkSenderGotImageFromStreamer(sender);

	CHECK_FALSE(waitForSignal(sender.get(), SIGNAL(newPackage())));

	imagestreamer->stopStreaming();
}

TEST_CASE("MHDImageStreamer: File should be read and send slices with a given interval", "[video][unit]")
{
	TestSenderPtr sender(new TestSender());
	cx::ImageStreamerPtr imagestreamer = createRunningImageStreamer(sender);

	CHECK(waitForSignal(sender.get(), SIGNAL(newPackage())));
	checkSenderGotImageFromStreamer(sender);

	CHECK(waitForSignal(sender.get(), SIGNAL(newPackage())));
	checkSenderGotImageFromStreamer(sender);

	imagestreamer->stopStreaming();
}



}//namespace cx
