
#include <QApplication>
#include <QMainWindow>
#include <iostream>
#include <boost/shared_ptr.hpp>
#include "sscDataManager.h"
#include "sscImage.h"
#include "sscVolumeRep.h"
#include "sscView.h"

/** Test app for SSC
 */
int main(int argc, char **argv)
{
	QApplication app(argc, argv);
	QMainWindow mainwindow;

	std::string testFile = //"/Users/frankl/fl/dev/Repository/data/nevro/MetaImage/3DT1.mhd";
	                       "/Users/frankl/fl/dev2/cvsStable/ssc/Test/TestData/Fantomer/Kaisa/MetaImage/Kaisa.mhd";
						   //"/Users/jbake/jbake/dev/ssc/Test/TestData/Fantomer/Kaisa/MetaImage/Kaisa.mhd";
	                       //"/home/christiana/snw2_trunk/C_code/SonowandSintefCooperation/test/TestData/Fantomer/Kaisa/MetaImage/Kaisa.mhd"
	ssc::ImagePtr imagePtr = ssc::DataManager::instance()->loadImage(testFile, ssc::rtMETAIMAGE);

	typedef boost::shared_ptr<class ssc::VolumeRep> VolumeRepPtr;
	VolumeRepPtr volumeRepPtr;
	volumeRepPtr.reset(new ssc::VolumeRep(imagePtr->getUid(), imagePtr->getName(),imagePtr));

	ssc::View view;
	view.setRep(volumeRepPtr);

	mainwindow.setCentralWidget(&view);
	mainwindow.show();

	int val = app.exec();
	return val;
	//return 0;
}
