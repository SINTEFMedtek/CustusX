/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
=========================================================================*/

#include "cxSmoothingImageFilter.h"

#include "cxAlgorithmHelpers.h"
#include <itkSmoothingRecursiveGaussianImageFilter.h>
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
	        "<p>Wrapper for a itk::SmoothingRecursiveGaussianImageFilter.</p>"
	        "<p>Computes the smoothing of an image by convolution with "
	        "the Gaussian kernels implemented as IIR filters."
	        "This filter is implemented using the recursive gaussian filters.</p>"
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

	itkImageType::ConstPointer itkImage = AlgorithmHelper::getITKfromSSCImage(input);

	typedef itk::SmoothingRecursiveGaussianImageFilter<itkImageType, itkImageType> smoothingFilterType;
	smoothingFilterType::Pointer smoohingFilter = smoothingFilterType::New();
	smoohingFilter->SetSigma(sigma->getValue());
	smoohingFilter->SetInput(itkImage);
	smoohingFilter->Update();
	itkImage = smoohingFilter->GetOutput();

	//Convert ITK to VTK
	itkToVtkFilterType::Pointer itkToVtkFilter = itkToVtkFilterType::New();
	itkToVtkFilter->SetInput(itkImage);
	itkToVtkFilter->Update();

	vtkImageDataPtr rawResult = vtkImageDataPtr::New();
	rawResult->DeepCopy(itkToVtkFilter->GetOutput());
	// TODO: possible memory problem here - check debug mem system of itk/vtk

	mRawResult =  rawResult;
	return true;
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

