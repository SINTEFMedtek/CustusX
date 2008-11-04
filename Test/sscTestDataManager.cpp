#include <iostream>
#include <string>

#include "sscTestUtilities.h"

/** Test app for SSC
 */
int main(int argc, char **argv)
{
	
	std::cout << ssc::TestUtilities::GetDataRoot() << std::endl;

	std::string testFile1 = TestUtilities::ExpandDataFileName("/Fantomer/Kaisa/MetaImage/Kaisa.mhd");
	std::string testFile2 = TestUtilities::ExpandDataFileName("/MetaImage/20070309T105136_MRT1.mhd");
	std::string testFile3 = TestUtilities::ExpandDataFileName("/MetaImage/20070309T102309_MRA.mhd");
	std::cout << testFile1 << std::endl;
	std::cout << testFile2 << std::endl;
	std::cout << testFile3 << std::endl;
	

	return 0;
}
