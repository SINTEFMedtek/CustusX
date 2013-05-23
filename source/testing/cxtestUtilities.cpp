#include "cxtestUtilities.h"

#include "vtkImageData.h"
#include "sscImage.h"

namespace cxtest
{

vtkImageDataPtr Utilities::create3DVtkImageData()
{
	vtkImageDataPtr vtkImageData = vtkImageDataPtr::New();

	vtkImageData->SetDimensions(10, 10, 10);
	vtkImageData->SetNumberOfScalarComponents(1);

	int* dims = vtkImageData->GetDimensions();

	for (int z  = 0; z < dims[2]; z++)
	{
		for (int y = 0; y < dims[1]; y++)
		{
			for (int x = 0; x < dims[0]; x++)
			{
				vtkImageData->SetScalarComponentFromDouble(x, y, z, 0, 2.0);
			}
		}
	}
//	vtkImageData->Print(std::cout);

	return vtkImageData;
}

ssc::ImagePtr Utilities::create3DImage()
{
	QString imagesUid("TESTUID");
	vtkImageDataPtr vtkImageData = create3DVtkImageData();
	ssc::ImagePtr image(new ssc::Image(imagesUid, vtkImageData));

	return image;
}

} /* namespace cxtest */
