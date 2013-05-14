#include "catch.hpp"

#include <QEventLoop>

#include <QFile>
#include <QString>
#include "sscImage.h"
#include "sscDataManager.h"
#include "cxDataLocations.h"
#include "cxMHDImageStreamer.h"
#include "cxTestSender.h"

namespace cxtest
{

///< Usage:
///< QObject* object = ...;
///< waitForSignal(object, SIGNAL(newPackage()));
void waitForSignal(QObject* object, const char* signal, int maxWaitMilliSeconds = 5000)
{
	QTimer* timer = new QTimer;
	timer->setInterval(maxWaitMilliSeconds);
	timer->start();

	QEventLoop loop;
	QObject::connect(timer, SIGNAL(timeout()), &loop, SLOT(quit()));
	QObject::connect(object, signal, &loop, SLOT(quit()));

	// Execute the event loop here, now we will wait here until the given signal is emitted
	// which in turn will trigger event loop quit.
	loop.exec();
}


TEST_CASE("File should be read and sent once", "[video][unit]")
{

	//import volume
	QString filename = cx::DataLocations::getTestDataPath()+ "/testing/TubeSegmentationFramework/Default.mhd";
	REQUIRE(QFile::exists(filename));

	//make imagestreamer
	cx::StringMap args;
	args["filename"] = filename;
	args["type"] = "MHDFile";
	args["singleshot"] = "true";

	cx::ImageStreamerPtr imagestreamer(new cx::MHDImageStreamer());
	imagestreamer->initialize(args);
	REQUIRE(imagestreamer);

	TestSenderPtr sender(new TestSender());

	CHECK(imagestreamer->startStreaming(sender));

	QObject* object = sender.get();
	waitForSignal(object, SIGNAL(newPackage()));

	//check that volume is sent from imagestreamer to the sender
	cx::PackagePtr package = sender->getPackage();
	REQUIRE(package);
	ssc::ImagePtr image = package->mImage;
	REQUIRE(image);

	waitForSignal(object, SIGNAL(newPackage()));

	imagestreamer->stopStreaming();
}

TEST_CASE("File should be read and slices with a given interval", "[video][unit]")
{
	//import volume
	QString filename = cx::DataLocations::getTestDataPath()+ "/testing/TubeSegmentationFramework/Default.mhd";
	REQUIRE(QFile::exists(filename));

	//make imagestreamer
	cx::StringMap args;
	args["filename"] = filename;
	args["type"] = "MHDFile";

	cx::ImageStreamerPtr imagestreamer(new cx::MHDImageStreamer());
	imagestreamer->initialize(args);
	REQUIRE(imagestreamer);

	TestSenderPtr sender(new TestSender());

	CHECK(imagestreamer->startStreaming(sender));

	QObject* object = sender.get();
	waitForSignal(object, SIGNAL(newPackage()));

	//check that volume is sent from imagestreamer to the sender
	cx::PackagePtr package = sender->getPackage();
	REQUIRE(package);
	ssc::ImagePtr image = package->mImage;
	REQUIRE(image);

	waitForSignal(object, SIGNAL(newPackage()));

	//check that volume is sent from imagestreamer to the sender
	cx::PackagePtr packageA = sender->getPackage();
	REQUIRE(packageA);
	ssc::ImagePtr imageA = package->mImage;
	REQUIRE(imageA);

	imagestreamer->stopStreaming();
}



}//namespace cx
