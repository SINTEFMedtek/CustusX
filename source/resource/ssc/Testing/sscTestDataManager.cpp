#include <iostream>
#include <string>
#include <vector>

#include <vtkImageData.h>

#include "sscTestUtilities.h"

#include "sscDataManager.h"
#include "sscImage.h"
#include "sscTypeConversions.h"

//namespace ssc
//{

/**Test loading images from file and inserting them into DataManager
 * 
 */
int main(int argc, char **argv)
{
	std::cout << ssc::TestUtilities::GetDataRoot() << std::endl;

//	QString testFile1 = ssc::TestUtilities::ExpandDataFileName("Fantomer/Kaisa/MetaImage/Kaisa.mhd");
	QString testFile2 = ssc::TestUtilities::ExpandDataFileName("Person5/person5_t2_unsigned.mhd");
	QString testFile3 = ssc::TestUtilities::ExpandDataFileName("Person5/person5_flair_unsigned.mhd");
//	std::cout << testFile1 << std::endl;
	std::cout << testFile2 << std::endl;
	//std::cout << testFile3 << std::endl;

//	ssc::ImagePtr image1 = ssc::DataManager::getInstance()->loadImage(testFile1, testFile1, ssc::rtMETAIMAGE);
	ssc::ImagePtr image2 = ssc::DataManager::getInstance()->loadImage(testFile2, testFile2, ssc::rtMETAIMAGE);
	ssc::ImagePtr image3 = ssc::DataManager::getInstance()->loadImage(testFile3, testFile3, ssc::rtMETAIMAGE);

//	QString Uid1 = image1->getUid();
	QString Uid2 = image2->getUid();
	QString Uid3 = image3->getUid();

	//ssc::ImagePtr imageTmp = ssc::DataManager::instance()->getImage(Uid1);

	std::cout << "Uid       : " << image2->getUid() << std::endl;
	std::cout << "Name      : " << image2->getName() << std::endl;
	std::cout << "RegistrationStatus: " << image2->getRegistrationStatus() << std::endl;

	int dims[3];
	double spacing[3];
	double origin[3];
	double center[3];
	int extent[6];
	double bounds[6];
	image2->getBaseVtkImageData()->GetDimensions(dims);
	image2->getBaseVtkImageData()->GetSpacing(spacing);
	image2->getBaseVtkImageData()->GetOrigin(origin);
	image2->getBaseVtkImageData()->GetCenter(center);
	image2->getBaseVtkImageData()->GetExtent(extent);
	image2->getBaseVtkImageData()->GetBounds(bounds);
	std::cout << "ScalarType: " << image2->getBaseVtkImageData()->GetScalarTypeAsString() << std::endl;
	std::cout << "Dims      : " << dims[0] << " / " << dims[1] << " / " << dims[2] << std::endl;
	std::cout << "Spacing   : " << spacing[0] << " / " << spacing[1] << " / " << spacing[2] << std::endl;
	std::cout << "Origin    : " << origin[0] << " / " << origin[1] << " / " << origin[2] << std::endl;
	std::cout << "Center    : " << center[0] << " / " << center[1] << " / " << center[2] << std::endl;
	std::cout << "Extent    : " << extent[0] << "/" << extent[1] << " / " << extent[2] << "/" << extent[3] << " / " << extent[4] << "/" << extent[5] << std::endl;
	std::cout << "Bounds    : " << bounds[0] << "/" << bounds[1] << " / " << bounds[2] << "/" << bounds[3] << " / " << bounds[4] << "/" << bounds[5] << std::endl;


	//std::vector<QString> names = ssc::DataManager::instance()->getImageNames();

	return 0;
}

//}//namespace ssc
