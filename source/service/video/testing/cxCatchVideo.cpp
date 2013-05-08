#include "catch.hpp"

#include <QFile>
#include <QString>
#include "sscImage.h"
#include "sscDataManager.h"
#include "cxDataLocations.h"
#include "cxMHDImageSender.h"
#include "cxGrabberSenderDirectLink.h"
#include "cxImageSenderFactory.h"
#include "cxTestGrabberSenderController.h"

namespace cx
{
TEST_CASE("Create image sender", "[video][unit]"){
	//import volume
	QString filename = cx::DataLocations::getTestDataPath()+ "/testing/TubeSegmentationFramework/Default.mhd";
	REQUIRE(QFile::exists(filename));

	//make imagesender
	StringMap args;
	args["filename"] = filename;
	args["type"] = "MHDFile";
	ImageStreamerPtr imagesender = cx::ImageSenderFactory().getFromArguments(args);
	REQUIRE(imagesender);

	GrabberSenderDirectLinkPtr grabbersender(new GrabberSenderDirectLink());
//	TestGrabberSenderController controller(NULL);
//	controller.initialize(grabbersender);

	//start imagesender
	CHECK(imagesender->startStreaming(grabbersender));

//	int count = 100000;
//	while(!controller.verify() && count > 0){
//		count--;
//	}

	//check that volume is sendt from imagesender
	ssc::ImagePtr image = grabbersender->popImage();
	REQUIRE(image);
}



}//namespace cx
