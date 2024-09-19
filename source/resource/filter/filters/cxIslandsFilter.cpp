/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include <random>
#include <cmath>

#include <vtkCellData.h>
#include <vtkUnsignedCharArray.h>
#include <vtkPolyData.h>
#include <vtkIdTypeArray.h>

#include "cxIslandsFilter.h"
#include "cxTypeConversions.h"
#include "cxSelectDataStringProperty.h"
#include "cxDoubleProperty.h"
#include "cxData.h"
#include "cxImage.h"
#include "cxVisServices.h"
#include "cxStringProperty.h"
#include "vtkForwardDeclarations.h"
#include "cxLogger.h"
#include "vtkImageConnectivityFilter.h"
#include "vtkImageData.h"
#include "cxPatientModelService.h"
#include "cxVolumeHelpers.h"

namespace cx
{

IslandsFilter::IslandsFilter(VisServicesPtr services) :
	FilterImpl(services)
{
}

QString IslandsFilter::getName() const
{
	return "Islands filter";
}

QString IslandsFilter::getType() const
{
	return "islands_filter";
}

QString IslandsFilter::getNameSuffixIslands()
{
	return "_islands";
}

QString IslandsFilter::getHelp() const
{
	return "A filter to separate a binary volume into labeled islands";
}

DoublePropertyPtr IslandsFilter::getMinimumSizeOption(QDomElement root)
{
	DoublePropertyPtr retval = DoubleProperty::initialize("Min size (voxels)", "",
															"Select the minimum island size in voxels", 1000, DoubleRange(1, 100000, 100), 0, root);
	return retval;
}

void IslandsFilter::createOptions()
{
	mOptionsAdapters.push_back(this->getMinimumSizeOption(mOptions));
}

void IslandsFilter::createInputTypes()
{
	SelectDataStringPropertyBasePtr temp;

	temp = StringPropertySelectImage::New(mServices->patient());
	temp->setValueName("Input");
	temp->setHelp("Input volume to divide into islands.");
	mInputTypes.push_back(temp);
}

void IslandsFilter::createOutputTypes()
{
	SelectDataStringPropertyBasePtr temp;

	temp = StringPropertySelectImage::New(mServices->patient());
	temp->setValueName("Output");
	temp->setHelp("Output labeled volume");
	mOutputTypes.push_back(temp);
}

bool IslandsFilter::execute()
{
	ImagePtr inputImage = boost::dynamic_pointer_cast<StringPropertySelectImage>(mInputTypes[0])->getImage();
	if (!inputImage)
		return false;
	
	int minimumSize = int(this->getMinimumSizeOption(mOptions)->getValue());

	QString uid = inputImage->getUid() + "_Islands%1";
	QString name = inputImage->getName()+" Islands%1";
	vtkImageDataPtr inputVtkImage = this->findIslandsInImage(inputImage->getBaseVtkImageData(), minimumSize);

	mOutputImage = this->execute(inputImage, inputVtkImage, uid, name, minimumSize);
	if(!mOutputImage)
		return false;

	if(mOutputTypes.size() > 0)
		mOutputTypes[0]->setValue(mOutputImage->getUid());
	return true;
}

ImagePtr IslandsFilter::execute(ImagePtr baseImage, vtkImageDataPtr inputVtkImage, QString uid, QString name, int minimumSize)
{
	if (!inputVtkImage)
		return ImagePtr();

	vtkImageDataPtr labeledImage = this->findIslandsInImage(inputVtkImage, minimumSize);
	if(!labeledImage)
		return ImagePtr();

	ImagePtr baseImageCopy = baseImage->copy();

	ImagePtr output = createDerivedImage(mServices->patient(),
																			 uid, name,
																			 labeledImage, baseImageCopy);
	output->mergevtkSettingsIntosscTransform();

	if (!output)
			return ImagePtr();

	mServices->patient()->insertData(output);

	// set output
//	mOutputTypes.front()->setValue(output->getUid());

	return output;
}


ImagePtr IslandsFilter::getOutputImage()
{
	return mOutputImage;
}

bool IslandsFilter::postProcess()
{
	if (mOutputTypes.front()->getData())
		mOutputTypes.front()->setValue(mOutputTypes.front()->getData()->getUid());

	return true;
}

std::vector<double> IslandsFilter::getIslandSizes()
{
	return mIslandSizes;
}


vtkImageDataPtr IslandsFilter::findIslandsInImage(vtkImageDataPtr image, int minimumSize)
{
	if(!image)
		return vtkImageDataPtr();

	vtkImageConnectivityFilter* connectivityFilterPtr = vtkImageConnectivityFilter::New();
	connectivityFilterPtr->SetInputData(image);
	connectivityFilterPtr->SetExtractionModeToAllRegions();
	connectivityFilterPtr->SetLabelModeToSizeRank();
	connectivityFilterPtr->SetScalarRange(1,1);
	connectivityFilterPtr->SetSizeRange(minimumSize, VTK_ID_MAX);
	connectivityFilterPtr->Update();
	vtkImageDataPtr labeledImage = connectivityFilterPtr->GetOutput();
	vtkIdTypeArray* regionSizes = connectivityFilterPtr->GetExtractedRegionSizes();

	mIslandSizes.clear();
	double* spacing = image->GetSpacing();
	double voxelVolume = spacing[0]*spacing[1]*spacing[2] / 1000; //cm³

	for(int i=0; i<regionSizes->GetSize(); i++)
	{
		mIslandSizes.push_back(regionSizes->GetValue(i)*voxelVolume);
		//CX_LOG_DEBUG() << "Region " << i << " - Voxels: " << regionSizes->GetValue(i) << "   Volume: " << mIslandSizes[i];
	}

	return labeledImage;
}


} // namespace cx
