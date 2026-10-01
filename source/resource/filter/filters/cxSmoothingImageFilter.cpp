/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxSmoothingImageFilter.h"

#include <vtkImageData.h>

#include <vtkImageCast.h>
#include <vtkImageGaussianSmooth.h>
#include "cxSelectDataStringProperty.h"

#include "cxUtilHelpers.h"
#include "cxRegistrationTransform.h"
#include "cxStringProperty.h"
#include "cxDoubleProperty.h"
#include "cxBoolProperty.h"
#include "cxTypeConversions.h"
#include "cxImage.h"

#include "cxPatientModelService.h"
#include "cxVolumeHelpers.h"
#include "cxVisServices.h"

namespace cx
{

SmoothingImageFilter::SmoothingImageFilter(VisServicesPtr services) :
	FilterImpl(services)
{
}

QString SmoothingImageFilter::getName() const
{
	return "Smoothing";
}

QString SmoothingImageFilter::getType() const
{
	return "smoothing_image_filter";
}

QString SmoothingImageFilter::getHelp() const
{
	return "<html>"
	        "<h3>Smoothing.</h3>"
	        "<p>Computes the smoothing of an image by convolution with "
	        "a Gaussian kernel. Sigma is the standard deviation in mm.</p>"
	        "</html>";
}

DoublePropertyPtr SmoothingImageFilter::getSigma(QDomElement root)
{
	return DoubleProperty::initialize("Smoothing sigma", "",
	                                             "Used for smoothing the segmented volume. Measured in units of image spacing.",
	                                             0.10, DoubleRange(0, 5, 0.01), 2, root);
}

void SmoothingImageFilter::createOptions()
{
	mOptionsAdapters.push_back(this->getSigma(mOptions));
}

void SmoothingImageFilter::createInputTypes()
{
	SelectDataStringPropertyBasePtr temp;

	temp = StringPropertySelectImage::New(mServices->patient());
	temp->setValueName("Input");
	temp->setHelp("Select image input for smoothing");
	mInputTypes.push_back(temp);
}

void SmoothingImageFilter::createOutputTypes()
{
	SelectDataStringPropertyBasePtr temp;

	temp = StringPropertySelectData::New(mServices->patient());
	temp->setValueName("Output");
	temp->setHelp("Output smoothed image");
	mOutputTypes.push_back(temp);
}

bool SmoothingImageFilter::execute()
{
	ImagePtr input = this->getCopiedInputImage();
	if (!input)
		return false;

	DoublePropertyPtr sigma = this->getSigma(mCopiedOptions);

	mRawResult = smooth(input->getBaseVtkImageData(), sigma->getValue());
	return true;
}

vtkImageDataPtr SmoothingImageFilter::smooth(vtkImageDataPtr image, double sigma)
{
	vtkImageCastPtr cast = vtkImageCastPtr::New();
	cast->SetInputData(image);
	cast->SetOutputScalarTypeToShort();

	const double* spacing = image->GetSpacing();
	vtkSmartPointer<vtkImageGaussianSmooth> smoothing = vtkSmartPointer<vtkImageGaussianSmooth>::New();
	smoothing->SetInputConnection(cast->GetOutputPort());
	smoothing->SetDimensionality(3);
	smoothing->SetStandardDeviations(sigma/spacing[0], sigma/spacing[1], sigma/spacing[2]);
	smoothing->SetRadiusFactors(3, 3, 3);
	smoothing->Update();
	return smoothing->GetOutput();
}

bool SmoothingImageFilter::postProcess()
{
	if (!mRawResult)
		return false;

	ImagePtr input = this->getCopiedInputImage();

	if (!input)
		return false;

	QString uid = input->getUid() + "_sm%1";
	QString name = input->getName()+" sm%1";
	ImagePtr output = createDerivedImage(mServices->patient(),
										 uid, name,
										 mRawResult, input);

	mRawResult = NULL;
	if (!output)
		return false;

	mServices->patient()->insertData(output);

	// set output
	mOutputTypes.front()->setValue(output->getUid());

	return true;
}


} // namespace cx

