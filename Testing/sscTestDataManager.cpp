#include <iostream>
#include <string>
#include <vector>

#include <vtkImageData.h>

#include "sscTestUtilities.h"

#include "sscDataManager.h"
#include "sscImage.h"

//namespace ssc
//{

int main(int argc, char **argv)
{
	std::cout << ssc::TestUtilities::GetDataRoot() << std::endl;

	std::string testFile1 = ssc::TestUtilities::ExpandDataFileName("Fantomer/Kaisa/MetaImage/Kaisa.mhd");
	std::string testFile2 = ssc::TestUtilities::ExpandDataFileName("MetaImage/20070309T105136_MRT1.mhd");
	std::string testFile3 = ssc::TestUtilities::ExpandDataFileName("MetaImage/20070309T102309_MRA.mhd");
	std::cout << testFile1 << std::endl;
	//std::cout << testFile2 << std::endl;
	//std::cout << testFile3 << std::endl;

	ssc::ImagePtr image1 = ssc::DataManager::instance()->loadImage(testFile1, ssc::rtMETAIMAGE);
	ssc::ImagePtr image2 = ssc::DataManager::instance()->loadImage(testFile2, ssc::rtMETAIMAGE);
	ssc::ImagePtr image3 = ssc::DataManager::instance()->loadImage(testFile3, ssc::rtMETAIMAGE);
	
	std::string Uid1 = image1->getUid();
	std::string Uid2 = image2->getUid();
	std::string Uid3 = image3->getUid();	
	
	//ssc::ImagePtr imageTmp = ssc::DataManager::instance()->getImage(Uid1);
	
	std::cout << "Uid       : " << image1->getUid() << std::endl;
	std::cout << "Name      : " << image1->getName() << std::endl;
	std::cout << "RegistrationStatus: " << image1->getRegistrationStatus() << std::endl;
	
	int dims[3];
	double spacing[3];
	double origin[3];
	double center[3];
	int extent[6];
	double bounds[6];
	image1->getVtkImageData()->GetDimensions(dims);
	image1->getVtkImageData()->GetSpacing(spacing);
	image1->getVtkImageData()->GetOrigin(origin);
	image1->getVtkImageData()->GetCenter(center);
	image1->getVtkImageData()->GetExtent(extent);
	image1->getVtkImageData()->GetBounds(bounds);
	std::cout << "ScalarType: " << image1->getVtkImageData()->GetScalarTypeAsString() << std::endl;
	std::cout << "Dims      : " << dims[0] << " / " << dims[1] << " / " << dims[2] << std::endl;	
	std::cout << "Spacing   : " << spacing[0] << " / " << spacing[1] << " / " << spacing[2] << std::endl;
	std::cout << "Origin    : " << origin[0] << " / " << origin[1] << " / " << origin[2] << std::endl;
	std::cout << "Center    : " << center[0] << " / " << center[1] << " / " << center[2] << std::endl;
	std::cout << "Extent    : " << extent[0] << "/" << extent[1] << " / " << extent[2] << "/" << extent[3] << " / " << extent[4] << "/" << extent[5] << std::endl;
	std::cout << "Bounds    : " << bounds[0] << "/" << bounds[1] << " / " << bounds[2] << "/" << bounds[3] << " / " << bounds[4] << "/" << bounds[5] << std::endl;

	
	//std::vector<std::string> names = ssc::DataManager::instance()->getImageNames();

	return 0;
}

//}//namespace ssc