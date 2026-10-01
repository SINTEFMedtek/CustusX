/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxDilationFilter.h"

#include <vtkImageData.h>

#include <vtkPolyData.h>

#include "cxDoubleProperty.h"
#include "cxColorProperty.h"
#include "cxBoolProperty.h"
#include "cxStringProperty.h"
#include "cxSelectDataStringProperty.h"

#include <vtkImageThreshold.h>
#include <vtkImageContinuousDilate3D.h>
#include "cxUtilHelpers.h"
#include "cxContourFilter.h"
#include "cxMesh.h"
#include "cxImage.h"
#include "cxPatientModelService.h"
#include "cxVolumeHelpers.h"
#include "cxVisServices.h"


namespace cx {

DilationFilter::DilationFilter(VisServicesPtr services) :
	FilterImpl(services)
{
}

QString DilationFilter::getName() const
{
	return "Dilation";
}

QString DilationFilter::getType() const
{
	return "dilation_filter";
}

QString DilationFilter::getHelp() const
{
	return "<html>"
	        "<h3>Dilation Filter.</h3>"
	        "<p>This filter dilates a binary volume with a given radius in mm.<p>"
	        "<p>The dilation is performed using a ball structuring element<p>"
	        "</html>";
}

DoublePropertyPtr DilationFilter::getDilationRadiusOption(QDomElement root)
{
	DoublePropertyPtr retval = DoubleProperty::initialize("Dilation radius (mm)", "",
    "Set dilation radius in mm", 1, DoubleRange(1, 20, 1), 0,
                    root);
	retval->setGuiRepresentation(DoublePropertyBase::grSLIDER);
	return retval;
}

BoolPropertyPtr DilationFilter::getGenerateSurfaceOption(QDomElement root)
{
	BoolPropertyPtr retval = BoolProperty::initialize("Generate Surface", "",
	                                                                        "Generate a surface of the output volume", true,
	                                                                            root);
	return retval;
}

ColorPropertyPtr DilationFilter::getColorOption(QDomElement root)
{
	return ColorProperty::initialize("Color", "",
	                                            "Color of output model.",
	                                            QColor("green"), root);
}


void DilationFilter::createOptions()
{
	mOptionsAdapters.push_back(this->getDilationRadiusOption(mOptions));
	mOptionsAdapters.push_back(this->getGenerateSurfaceOption(mOptions));
	mOptionsAdapters.push_back(this->getColorOption(mOptions));
}

void DilationFilter::createInputTypes()
{
	SelectDataStringPropertyBasePtr temp;

	temp = StringPropertySelectImage::New(mServices->patient());
	temp->setValueName("Input");
	temp->setHelp("Select segmentation input for dilation");
	mInputTypes.push_back(temp);
}

void DilationFilter::createOutputTypes()
{
	SelectDataStringPropertyBasePtr temp;

	temp = StringPropertySelectData::New(mServices->patient());
	temp->setValueName("Output");
	temp->setHelp("Dilated segmentation image");
	mOutputTypes.push_back(temp);

	temp = StringPropertySelectData::New(mServices->patient());
	temp->setValueName("Contour");
	temp->setHelp("Output contour generated from dilated segmentation image.");
	mOutputTypes.push_back(temp);
}

bool DilationFilter::preProcess() {

    return FilterImpl::preProcess();
}

bool DilationFilter::execute() {
	ImagePtr input = this->getCopiedInputImage();
	if (!input)
		return false;

	double radius = this->getDilationRadiusOption(mCopiedOptions)->getValue();

	mRawResult = dilate(input->getBaseVtkImageData(), radius);

	BoolPropertyPtr generateSurface = this->getGenerateSurfaceOption(mCopiedOptions);
	if (generateSurface->getValue())
	{
        double threshold = 1;/// because the segmented image is 0..1
        mRawContour = ContourFilter::execute(mRawResult, threshold);
	}

    return true;
}

vtkImageDataPtr DilationFilter::dilate(vtkImageDataPtr image, double radius)
{
	vtkSmartPointer<vtkImageThreshold> binary = vtkSmartPointer<vtkImageThreshold>::New();
	binary->SetInputData(image);
	binary->ThresholdBetween(1, 1);
	binary->SetInValue(1);
	binary->SetOutValue(0);
	binary->ReplaceInOn();
	binary->ReplaceOutOn();
	binary->SetOutputScalarTypeToUnsignedChar();

	const double* spacing = image->GetSpacing();
	int kernelSize[3];
	for (int i = 0; i < 3; ++i)
	{
		const int radiusInVoxels = static_cast<int>(radius/spacing[i]);
		kernelSize[i] = 2*radiusInVoxels + 1;
	}
	vtkSmartPointer<vtkImageContinuousDilate3D> dilation = vtkSmartPointer<vtkImageContinuousDilate3D>::New();
	dilation->SetInputConnection(binary->GetOutputPort());
	dilation->SetKernelSize(kernelSize[0], kernelSize[1], kernelSize[2]);
	dilation->Update();
	return dilation->GetOutput();
}

bool DilationFilter::postProcess()
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
	if (!output)
		return false;

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



} // namespace cx
