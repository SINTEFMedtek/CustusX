
#include <QApplication>
#include <iostream>
#include "sscDataManager.h"

/** Test app for SSC
 */
int main(int argc, char **argv)
{
	QApplication app(argc, argv);
	
	std::string testFile = "/home/christiana/snw2_trunk/C_code/SonowandSinfefCooperation/test/TestData/Fantomer/Kaisa/MetaImage/Kaisa.mhd";
	                       //"/home/christiana/snw2_trunk/C_code/SonowandSintefCooperation/test/TestData/Fantomer/Kaisa/MetaImage/Kaisa.mhd" 
	ssc::DataManager::instance()->loadImage(testFile, ssc::rtMETAIMAGE);
	
	//int val = app.exec();
	//return val;
	return 0;
}
