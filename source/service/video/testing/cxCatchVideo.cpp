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

TEST_CASE("MHDImageStreamer: File should be read and sent only once", "[video][unit]")
{
	QString filename = cx::DataLocations::getTestDataPath()+ "/testing/TubeSegmentationFramework/Default.mhd";
	REQUIRE(QFile::exists(filename));

	cx::StringMap args;
	args["filename"] = filename;
	args["type"] = "MHDFile";
	args["sendonce"] = "true";

	cx::ImageStreamerPtr imagestreamer(new cx::MHDImageStreamer());
	REQUIRE(imagestreamer);
	imagestreamer->initialize(args);

	TestSenderPtr sender(new TestSender());

	CHECK(imagestreamer->startStreaming(sender));

	QObject* object = sender.get();
	CHECK(waitForSignal(object, SIGNAL(newPackage())));

	cx::PackagePtr package = sender->getPackage();
	REQUIRE(package);
	ssc::ImagePtr image = package->mImage;
	REQUIRE(image);

	CHECK_FALSE(waitForSignal(object, SIGNAL(newPackage())));

	imagestreamer->stopStreaming();
}

TEST_CASE("MHDImageStreamer: File should be read and send slices with a given interval", "[video][unit]")
{
	QString filename = cx::DataLocations::getTestDataPath()+ "/testing/TubeSegmentationFramework/Default.mhd";
	REQUIRE(QFile::exists(filename));

	cx::StringMap args;
	args["filename"] = filename;
	args["type"] = "MHDFile";

	cx::ImageStreamerPtr imagestreamer(new cx::MHDImageStreamer());
	REQUIRE(imagestreamer);
	imagestreamer->initialize(args);

	TestSenderPtr sender(new TestSender());

	CHECK(imagestreamer->startStreaming(sender));

	QObject* object = sender.get();
	CHECK(waitForSignal(object, SIGNAL(newPackage())));

	cx::PackagePtr package = sender->getPackage();
	REQUIRE(package);
	ssc::ImagePtr image = package->mImage;
	REQUIRE(image);

	CHECK(waitForSignal(object, SIGNAL(newPackage())));

	package = sender->getPackage();
	REQUIRE(package);
	image = package->mImage;
	REQUIRE(image);

	imagestreamer->stopStreaming();
}



}//namespace cx
