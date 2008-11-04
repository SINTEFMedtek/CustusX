#include <iostream>
#include <string>

#include "sscTestUtilities.h"

#include "sscDataManager.h"
#include "sscImage.h"

//namespace ssc
//{

int main(int argc, char **argv)
{

	std::cout << ssc::TestUtilities::GetDataRoot() << std::endl;

	std::string testFile1 = ssc::TestUtilities::ExpandDataFileName("/Fantomer/Kaisa/MetaImage/Kaisa.mhd");
	std::string testFile2 = ssc::TestUtilities::ExpandDataFileName("/MetaImage/20070309T105136_MRT1.mhd");
	std::string testFile3 = ssc::TestUtilities::ExpandDataFileName("/MetaImage/20070309T102309_MRA.mhd");
	std::cout << testFile1 << std::endl;
	std::cout << testFile2 << std::endl;
	std::cout << testFile3 << std::endl;

	//ssc::ImagePtr image1 = ssc::DataManager::instance()->loadImage(testFile1, ssc::rtMETAIMAGE);

	return 0;
}

//}//namespace ssc