/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
	return "resample_image_filter";
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

