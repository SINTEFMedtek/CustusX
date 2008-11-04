#include <string>

#include <QApplication>

//#include "vtkTestUtilities.h"
#include "sscConfig.h"
#include "sscTestUtilities.h"

#include "sscTestInterface.h"

/** Test app for SSC
 */
int main(int argc, char **argv)
{

	//char* fname = vtkTestUtilities::ExpandDataFileName(argc, argv, "Data/combxyz.bin");
    //std::cout << "fname: " << fname << std::endl;
	std::string dataRoot = SSC_DATA_ROOT;
	std::string fileName = dataRoot + "Fantomer/Kaisa/MetaImage/Kaisa.mhd";
	cout << fileName << endl;

	std::cout << ssc::TestUtilities::GetDataRoot() << std::endl;
	std::cout << ssc::TestUtilities::ExpandDataFileName("Fantomer/Kaisa/MetaImage/Kaisa.mhd") << std::endl;

	QApplication app(argc, argv);

	ssc::TestInterface testInterface;

	int val = app.exec();
	return val;
	//return 0;
}
