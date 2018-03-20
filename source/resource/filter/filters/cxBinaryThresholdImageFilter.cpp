/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxBinaryThresholdImageFilter.h"

#include "cxAlgorithmHelpers.h"
#include <itkBinaryThresholdImageFilter.h>
#include <vtkImageCast.h>
#include "cxUtilHelpers.h"
#include "cxRegistrationTransform.h"
#include "cxStringProperty.h"
#include "cxColorProperty.h"
#include "cxBoolProperty.h"
#include "cxTypeConversions.h"
#include "cxDoublePairProperty.h"
#include "cxContourFilter.h"
#include "cxMesh.h"
#include "cxImage.h"
#include "cxSelectDataStringProperty.h"
#include "cxPatientModelService.h"
#include "cxViewService.h"
#include "cxVolumeHelpers.h"
#include "cxVisServices.h"

namespace cx
{

BinaryThresholdImageFilter::BinaryThresholdImageFilter(VisServicesPtr services) :
	FilterImpl(services)
{
}

QString BinaryThresholdImageFilter::getName() const
{
	return "Segmentation";
}

QString BinaryThresholdImageFilter::getType() const
{
	return "binary_threshold_image_filter";
}

QString BinaryThresholdImageFilter::getHelp() const
{
	return "<html>"
	        "<h3>Binary Threshold Image Filter.</h3>"
	        "<p><i>Segment out areas from the selected image using a threshold.</i></p>"
	        "<p>This filter produces an output image whose pixels are either one of two values"
	        "( OutsideValue or InsideValue ), depending on whether the corresponding input"
	        "image pixels lie between the two thresholds ( LowerThreshold and UpperThreshold )."
	        "Values equal to either threshold is considered to be between the thresholds.<p>"
	        "</html>";
}

DoublePairPropertyPtr BinaryThresholdImageFilter::getThresholdOption(QDomElement root)
{
	DoublePairPropertyPtr retval = DoublePairProperty::initialize("Thresholds", "",
                                                                              "Select the lower and upper thresholds for the segmentation", DoubleRange(0, 100, 1), 0,
																			  root);
	return retval;
}

BoolPropertyPtr BinaryThresholdImageFilter::getGenerateSurfaceOption(QDomElement root)
{
	BoolPropertyPtr retval = BoolProperty::initialize("Generate Surface", "",
																  "Generate a surface of the output volume", true,
																  root);
	return retval;
}

ColorPropertyPtr BinaryThresholdImageFilter::getColorOption(QDomElement root)
{
	return ColorProperty::initialize("Color", "",
                                                "The color of the output model.",
	                                            QColor("green"), root);
}

void BinaryThresholdImageFilter::createOptions()
{
	mThresholdOption = this->getThresholdOption(mOptions);
	connect(mThresholdOption.get(), &Property::changed, this, &BinaryThresholdImageFilter::thresholdSlot);
	mOptionsAdapters.push_back(mThresholdOption);
	mOptionsAdapters.push_back(this->getGenerateSurfaceOption(mOptions));
	mOptionsAdapters.push_back(this->getColorOption(mOptions));
}

void BinaryThresholdImageFilter::createInputTypes()
{
	SelectDataStringPropertyBasePtr temp;

	temp = StringPropertySelectImage::New(mServices->patient());
	temp->setValueName("Input");
	temp->setHelp("Select image input for thresholding");
	connect(temp.get(), SIGNAL(dataChanged(QString)), this, SLOT(imageChangedSlot(QString)));
	mInputTypes.push_back(temp);
}

void BinaryThresholdImageFilter::createOutputTypes()
{
	SelectDataStringPropertyBasePtr temp;

	temp = StringPropertySelectData::New(mServices->patient());
	temp->setValueName("Output");
	temp->setHelp("Output thresholded binary image");
	mOutputTypes.push_back(temp);

	temp = StringPropertySelectData::New(mServices->patient());
	temp->setValueName("Contour");
	temp->setHelp("Output contour generated from thresholded binary image.");
	mOutputTypes.push_back(temp);
}

void BinaryThresholdImageFilter::setActive(bool on)
{
	FilterImpl::setActive(on);

	if (!mActive)
		this->stopPreview();
}

void BinaryThresholdImageFilter::imageChangedSlot(QString uid)
{
	this->stopPreview();
	this->updateThresholdPairFromImageChange(uid, mThresholdOption);
}

void BinaryThresholdImageFilter::stopPreview()
{
	if(mPreviewImage)
		mPreviewImage->stopThresholdPreview();
	mPreviewImage.reset();
}

void BinaryThresholdImageFilter::thresholdSlot()
{
	if (mActive)
	{
		mPreviewImage = boost::dynamic_pointer_cast<Image>(mInputTypes[0]->getData());
		if(!mPreviewImage)
			return;
		Eigen::Vector2d threshold = Eigen::Vector2d(mThresholdOption->getValue()[0],  mThresholdOption->getValue()[1]);
		mPreviewImage->startThresholdPreview(threshold);
	}
}

bool BinaryThresholdImageFilter::preProcess()
{
	this->stopPreview();
	return FilterImpl::preProcess();

}

bool BinaryThresholdImageFilter::execute()
{
	ImagePtr input = this->getCopiedInputImage();
	if (!input)
		return false;

	DoublePairPropertyPtr thresholds = this->getThresholdOption(mCopiedOptions);
	BoolPropertyPtr generateSurface = this->getGenerateSurfaceOption(mCopiedOptions);

	itkImageType::ConstPointer itkImage = AlgorithmHelper::getITKfromSSCImage(input);

	//Binary Thresholding
	typedef itk::BinaryThresholdImageFilter<itkImageType, itkImageType> thresholdFilterType;
	thresholdFilterType::Pointer thresholdFilter = thresholdFilterType::New();
	thresholdFilter->SetInput(itkImage);
	thresholdFilter->SetOutsideValue(0);
	thresholdFilter->SetInsideValue(1);
	thresholdFilter->SetLowerThreshold(thresholds->getValue()[0]);
	thresholdFilter->SetUpperThreshold(thresholds->getValue()[1]);
	thresholdFilter->Update();
	itkImage = thresholdFilter->GetOutput();

	//Convert ITK to VTK
	itkToVtkFilterType::Pointer itkToVtkFilter = itkToVtkFilterType::New();
	itkToVtkFilter->SetInput(itkImage);
	itkToVtkFilter->Update();

	vtkImageDataPtr rawResult = vtkImageDataPtr::New();
	rawResult->DeepCopy(itkToVtkFilter->GetOutput());

	vtkImageCastPtr imageCast = vtkImageCastPtr::New();
	imageCast->SetInputData(rawResult);
	imageCast->SetOutputScalarTypeToUnsignedChar();
	imageCast->Update();
	rawResult = imageCast->GetOutput();

	// TODO: possible memory problem here - check debug mem system of itk/vtk

	mRawResult =  rawResult;

	if (generateSurface->getValue())
	{
		double threshold = 1;/// because the segmented image is 0..1
		mRawContour = ContourFilter::execute(mRawResult, threshold);
	}

	return true;
}

bool BinaryThresholdImageFilter::postProcess()
{
	if (!mRawResult)
		return false;

	ImagePtr input = this->getCopiedInputImage();

	if (!input)
		return false;

	QString uid = input->getUid() + "_seg%1";
	QString name = input->getName()+" seg%1";
	ImagePtr output = createDerivedImage(mServices->patient(),
										 uid, name,
										 mRawResult, input);

	mRawResult = NULL;

	output->setInitialWindowLevel(-1, -1);
	output->resetTransferFunctions();
	mServices->patient()->insertData(output);

	// set output
	mOutputTypes.front()->setValue(output->getUid());

	// set contour output
	if (mRawContour!=NULL)
	{
		ColorPropertyPtr colorOption = this->getColorOption(mOptions);
		MeshPtr contour = ContourFilter::postProcess(mServices->patient(), mRawContour, output, colorOption->getValue());
		mOutputTypes[1]->setValue(contour->getUid());
		mRawContour = vtkPolyDataPtr();
	}

	return true;
}


}//namespace cx
