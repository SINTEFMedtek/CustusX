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

TEST_CASE("File should be read and sent only once", "[video][unit]")
{
	QString filename = cx::DataLocations::getTestDataPath()+ "/testing/TubeSegmentationFramework/Default.mhd";
	REQUIRE(QFile::exists(filename));

	cx::StringMap args;
	args["filename"] = filename;
	args["type"] = "MHDFile";
	args["sendonce"] = "true";

	cx::ImageStreamerPtr imagestreamer(new cx::MHDImageStreamer());
	imagestreamer->initialize(args);
	REQUIRE(imagestreamer);

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

TEST_CASE("File should be read and slices with a given interval", "[video][unit]")
{
	QString filename = cx::DataLocations::getTestDataPath()+ "/testing/TubeSegmentationFramework/Default.mhd";
	REQUIRE(QFile::exists(filename));

	cx::StringMap args;
	args["filename"] = filename;
	args["type"] = "MHDFile";

	cx::ImageStreamerPtr imagestreamer(new cx::MHDImageStreamer());
	imagestreamer->initialize(args);
	REQUIRE(imagestreamer);

	TestSenderPtr sender(new TestSender());

	CHECK(imagestreamer->startStreaming(sender));

	QObject* object = sender.get();
	CHECK(waitForSignal(object, SIGNAL(newPackage())));

	cx::PackagePtr package = sender->getPackage();
	REQUIRE(package);
	ssc::ImagePtr image = package->mImage;
	REQUIRE(image);

	CHECK(waitForSignal(object, SIGNAL(newPackage())));

	cx::PackagePtr packageA = sender->getPackage();
	REQUIRE(packageA);
	ssc::ImagePtr imageA = package->mImage;
	REQUIRE(imageA);

	imagestreamer->stopStreaming();
}



}//namespace cx
