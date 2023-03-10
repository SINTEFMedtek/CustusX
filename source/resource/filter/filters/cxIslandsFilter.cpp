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
	
	mOutputImage = this->execute(inputImage, minimumSize);

	if(mOutputTypes.size() > 0)
		mOutputTypes[0]->setValue(mOutputImage->getUid());
	return true;
}

ImagePtr IslandsFilter::execute(ImagePtr inputImage, int minimumSize)
{
	if (!inputImage)
		return ImagePtr();


	ImagePtr outputImage;
	return outputImage;
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


} // namespace cx
