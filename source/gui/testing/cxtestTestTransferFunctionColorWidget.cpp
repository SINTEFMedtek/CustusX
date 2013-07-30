#include "cxtestTestTransferFunctionColorWidget.h"

#include <vtkImageData.h>
#include "sscImage.h"
#include "sscImageTF3D.h"

namespace cxtest {

// Similar to vtkImageDataPtr ApplyLUTToImage2DProxy::createDummyImageData()
// in file sscSlicedImageProxy.h  <== Reuse code
vtkImageDataPtr createDummyImageData(int axisSize, int maxVoxelValue)
{
	int size = axisSize - 1;//Modify axis size as extent starts with 0, not 1
	vtkImageDataPtr dummyImageData = vtkImageDataPtr::New();
	dummyImageData->SetExtent(0, size, 0, size, 0, size);
	dummyImageData->SetSpacing(1, 1, 1);
	//dummyImageData->SetScalarTypeToUnsignedShort();
	dummyImageData->SetScalarTypeToUnsignedChar();
	dummyImageData->SetNumberOfScalarComponents(1);
	dummyImageData->AllocateScalars();
	unsigned char* dataPtr = static_cast<unsigned char*> (dummyImageData->GetScalarPointer());

	//Init voxel colors
	int minVoxelValue = 0;
	int numVoxels = size*size*size;
	for (int i = 0; i < numVoxels; ++i)
	{
		int voxelValue = minVoxelValue + i;
		if (i == numVoxels)
			dataPtr[i] = maxVoxelValue;
		else if (voxelValue < maxVoxelValue)
			dataPtr[i] = voxelValue;
		else
			dataPtr[i] = maxVoxelValue;
	}
	return dummyImageData;
}

TestTransferFunctionColorWidget::TestTransferFunctionColorWidget() :
		TransferFunctionColorWidget(NULL)
{}


void TestTransferFunctionColorWidget::initWithTestData()
{
	vtkImageDataPtr dummyImageData = createDummyImageData(2, 1);
	ssc::ImagePtr dummyImage(new ssc::Image("DummyImage", dummyImageData));
	ssc::ImageTFDataPtr transferFunction;
	transferFunction = dummyImage->getTransferFunctions3D();
//	transferFunction->setWindow(1);
//	transferFunction->setLevel(0.5);
	this->setData(dummyImage, transferFunction);
	this->calculateColorTFBoundaries(
			this->mLeftAreaBoundary,
			this->mRightAreaBoundary,
			this->mWidthAreaBoundary);
}

void TestTransferFunctionColorWidget::setPlotArea(QRect plotArea)
{
	this->mPlotArea = plotArea;
}

int TestTransferFunctionColorWidget::getLeftAreaBoundary()
{
	return this->mLeftAreaBoundary;
}
int TestTransferFunctionColorWidget::getRigthAreaBoundary()
{
	return this->mRightAreaBoundary;
}

} /* namespace cxtest */
