#include "catch.hpp"

#include <QFile>
#include <QString>
#include "sscImage.h"
#include "sscDataManager.h"
#include "cxDataLocations.h"
#include "cxMHDImageStreamer.h"
#include "cxTestSender.h"

namespace cx
{
TEST_CASE("Create image sender", "[video][unit]"){

	//import volume
	QString filename = cx::DataLocations::getTestDataPath()+ "/testing/TubeSegmentationFramework/Default.mhd";
	REQUIRE(QFile::exists(filename));

	//make imagestreamer
	StringMap args;
	args["filename"] = filename;
	args["type"] = "MHDFile";
	args["singleshot"] = "true";

	ImageStreamerPtr imagestreamer(new MHDImageStreamer());
	imagestreamer->initialize(args);
	REQUIRE(imagestreamer);

	TestSenderPtr sender(new TestSender());

	CHECK(imagestreamer->startStreaming(sender));
	imagestreamer->stopStreaming();

	//check that volume is sent from imagestreamer
	PackagePtr package = sender->getPackage();
	REQUIRE(package);
	ssc::ImagePtr image = package->mImage;
	REQUIRE(image);
}



}//namespace cx
