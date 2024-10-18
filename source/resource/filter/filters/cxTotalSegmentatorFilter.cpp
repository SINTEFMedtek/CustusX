/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/


#include "cxTotalSegmentatorFilter.h"
#include "cxSelectDataStringProperty.h"
#include "cxData.h"
#include "cxImage.h"
#include "cxVisServices.h"
#include "cxStringProperty.h"
#include "vtkForwardDeclarations.h"
#include "cxLogger.h"
#include "vtkImageData.h"
#include "cxPatientModelService.h"
#include "cxVolumeHelpers.h"

namespace cx
{

TotalSegmentatorFilter::TotalSegmentatorFilter(VisServicesPtr services) :
	FilterImpl(services)
{
}

QString TotalSegmentatorFilter::getName() const
{
	return "Total Segmentator filter";
}

QString TotalSegmentatorFilter::getType() const
{
	return "total_segmantator_filter";
}

QString TotalSegmentatorFilter::getNameSuffixTotalSegmentator()
{
	return "_TSEGM";
}

QString TotalSegmentatorFilter::getHelp() const
{
	return "A filter to segment structures form CT or MR";
}


void TotalSegmentatorFilter::createOptions()
{
	//mOptionsAdapters.push_back(this->getMinimumSizeOption(mOptions));
}

void TotalSegmentatorFilter::createInputTypes()
{
	SelectDataStringPropertyBasePtr temp;

	temp = StringPropertySelectImage::New(mServices->patient());
	temp->setValueName("Input");
	temp->setHelp("Input volume to be segmented.");
	mInputTypes.push_back(temp);
}

void TotalSegmentatorFilter::createOutputTypes()
{
	SelectDataStringPropertyBasePtr temp;

	temp = StringPropertySelectImage::New(mServices->patient());
	temp->setValueName("Output");
	temp->setHelp("Output labeled volume");
	mOutputTypes.push_back(temp);
}

bool TotalSegmentatorFilter::execute()
{

	return true;
}

ImagePtr TotalSegmentatorFilter::execute(ImagePtr baseImage, vtkImageDataPtr inputVtkImage, QString uid, QString name, int minimumSize)
{
//	if (!inputVtkImage)
//		return ImagePtr();

//	vtkImageDataPtr labeledImage = this->findIslandsInImage(inputVtkImage, minimumSize);
//	if(!labeledImage)
//		return ImagePtr();

//	ImagePtr baseImageCopy = baseImage->copy();

//	ImagePtr output = createDerivedImage(mServices->patient(),
//																			 uid, name,
//																			 labeledImage, baseImageCopy);
//	output->mergevtkSettingsIntosscTransform();

//	if (!output)
//			return ImagePtr();

//	mServices->patient()->insertData(output);

	// set output

	ImagePtr output;
	return output;
}


bool TotalSegmentatorFilter::postProcess()
{
	if (mOutputTypes.front()->getData())
		mOutputTypes.front()->setValue(mOutputTypes.front()->getData()->getUid());

	return true;
}


} // namespace cx
