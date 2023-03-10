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
	
	mOutputImage = this->execute(inputImage, minimumSize);

	if(mOutputTypes.size() > 0)
		mOutputTypes[0]->setValue(mOutputImage->getUid());
	return true;
}

ImagePtr IslandsFilter::execute(ImagePtr inputImage, int minimumSize)
{
	if (!inputImage)
		return ImagePtr();

	vtkImageDataPtr labeledImage = this->findIslandsInImage(inputImage->getBaseVtkImageData(), minimumSize);
	if(!labeledImage)
		return ImagePtr();

	ImagePtr input = this->getCopiedInputImage(0);

	QString uid = input->getUid() + "_Islands%1";
	QString name = input->getName()+" Islands%1";
	ImagePtr output = createDerivedImage(mServices->patient(),
																			 uid, name,
																			 labeledImage, input);
	output->mergevtkSettingsIntosscTransform();

	if (!output)
			return ImagePtr();

	mServices->patient()->insertData(output);

	// set output
	mOutputTypes.front()->setValue(output->getUid());

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

	return labeledImage;
}


} // namespace cx
