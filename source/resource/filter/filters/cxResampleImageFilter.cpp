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

#include "cxResampleImageFilter.h"

#include <QApplication>

#include "cxImageAlgorithms.h"
#include "cxImage.h"
#include "vtkImageData.h"
#include "cxSelectDataStringProperty.h"
#include "cxDoubleProperty.h"
#include "cxPatientModelService.h"
#include "cxVisServices.h"

namespace cx
{

ResampleImageFilter::ResampleImageFilter(VisServicesPtr services) :
	FilterImpl(services)
{
}

QString ResampleImageFilter::getName() const
{
	return "Resample";
}

QString ResampleImageFilter::getType() const
{
	return "ResampleImageFilter";
}

QString ResampleImageFilter::getHelp() const
{
	return "<html>"
	        "<h3>Resample.</h3>"
	        "<p><i>Resample the volume into the space of the reference volume. Also crop to the same volume.</i></p>"
	        "</html>";
}

DoublePropertyPtr ResampleImageFilter::getMarginOption(QDomElement root)
{
	return DoubleProperty::initialize("Margin", "",
	                                             "mm Margin added to ref image bounding box",
	                                             5.0, DoubleRange(0, 50, 1), 1, root);
}

void ResampleImageFilter::createOptions()
{
	mOptionsAdapters.push_back(this->getMarginOption(mOptions));
}

void ResampleImageFilter::createInputTypes()
{
	SelectDataStringPropertyBasePtr temp;

	temp = StringPropertySelectImage::New(mServices->patient());
	temp->setValueName("Input");
	temp->setHelp("Select input to be resampled");
	mInputTypes.push_back(temp);

	temp = StringPropertySelectImage::New(mServices->patient());
	temp->setValueName("Reference");
	temp->setHelp("Select reference. Resample input into this coordinate system and bounding box");
	mInputTypes.push_back(temp);
}

void ResampleImageFilter::createOutputTypes()
{
	SelectDataStringPropertyBasePtr temp;

	temp = StringPropertySelectData::New(mServices->patient());
	temp->setValueName("Output");
	temp->setHelp("Output thresholded binary image");
	mOutputTypes.push_back(temp);
}

//bool ResampleImageFilter::preProcess()
//{
//    return FilterImpl::preProcess();
//}

/** Crop the image to the bounding box bb_q.
 *  bb_q is given in the output space q, defined relative to the image space d
 *  with qMd. If qMd is non-identity, image is resampled to space q.
 *  outputSpacing can be used to resample the volume (after cropping).
 */
bool ResampleImageFilter::execute()
{
	ImagePtr input = this->getCopiedInputImage(0);
	ImagePtr reference = this->getCopiedInputImage(1);
	if (!input || !reference)
		return false;

	DoublePropertyPtr marginOption = this->getMarginOption(mCopiedOptions);
	double margin = marginOption->getValue();

	Transform3D refMi = reference->get_rMd().inv() * input->get_rMd();
	ImagePtr oriented = resampleImage(mServices->patient(), input, refMi);//There is an error with the transfer functions in this image

	Transform3D orient_M_ref = oriented->get_rMd().inv() * reference->get_rMd();
	DoubleBoundingBox3D bb_crop = transform(orient_M_ref, reference->boundingBox());

	// increase bb size by margin
	bb_crop[0] -= margin;
	bb_crop[1] += margin;
	bb_crop[2] -= margin;
	bb_crop[3] += margin;
	bb_crop[4] -= margin;
	bb_crop[5] += margin;

	oriented->setCroppingBox(bb_crop);

	ImagePtr cropped = cropImage(mServices->patient(), oriented);

	QString uid = input->getUid() + "_resample%1";
	QString name = input->getName() + " resample%1";

	ImagePtr resampled = resampleImage(mServices->patient(), cropped, Vector3D(reference->getBaseVtkImageData()->GetSpacing()), uid, name);

	// important! move thread affinity to main thread - ensures signals/slots is still called correctly
	resampled->moveThisAndChildrenToThread(QApplication::instance()->thread());

	mRawResult =  resampled;
	return true;
}

bool ResampleImageFilter::postProcess()
{
	if (!mRawResult)
		return false;

	ImagePtr output = mRawResult;
	mRawResult.reset();
	mServices->patient()->insertData(output);

	// set output
	mOutputTypes.front()->setValue(output->getUid());
	return true;
}



} // namespace cx

