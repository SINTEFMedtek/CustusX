#include "cxDilationFilter.h"

#include "sscDoubleDataAdapterXml.h"
#include "sscColorDataAdapterXml.h"
#include "sscBoolDataAdapterXml.h"
#include "sscStringDataAdapterXml.h"
#include "cxSelectDataStringDataAdapter.h"

#include <itkBinaryDilateImageFilter.h>
#include <itkBinaryBallStructuringElement.h>
#include "cxAlgorithmHelpers.h"
#include <vtkImageCast.h>
#include "sscDataManager.h"
#include "sscUtilHelpers.h"
#include "cxContourFilter.h"
#include "sscMesh.h"
#include "sscImage.h"
#include "sscDataManager.h"
#include "cxPatientService.h"
#include "cxPatientData.h"

namespace cx {

QString DilationFilter::getName() const
{
	return "Dilation";
}

QString DilationFilter::getType() const
{
	return "DilationFilter";
}

QString DilationFilter::getHelp() const
{
	return "<html>"
	        "<h3>Dilation Filter.</h3>"
	        "<p>This filter dilates a binary volume with a given radius in mm.<p>"
	        "<p>The dilation is performed using a ball structuring element<p>"
	        "</html>";
}

DoubleDataAdapterXmlPtr DilationFilter::getDilationRadiusOption(QDomElement root)
{
	DoubleDataAdapterXmlPtr retval = DoubleDataAdapterXml::initialize("Dilation radius (mm)", "",
    "Set dilation radius in mm", 1, DoubleRange(1, 20, 1), 0,
                    root);
	retval->setAddSlider(true);
	return retval;
}

BoolDataAdapterXmlPtr DilationFilter::getGenerateSurfaceOption(QDomElement root)
{
	BoolDataAdapterXmlPtr retval = BoolDataAdapterXml::initialize("Generate Surface", "",
	                                                                        "Generate a surface of the output volume", true,
	                                                                            root);
	return retval;
}

ColorDataAdapterXmlPtr DilationFilter::getColorOption(QDomElement root)
{
	return ColorDataAdapterXml::initialize("Color", "",
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
	SelectDataStringDataAdapterBasePtr temp;

	temp = SelectImageStringDataAdapter::New();
	temp->setValueName("Input");
	temp->setHelp("Select segmentation input for dilation");
	mInputTypes.push_back(temp);
}

void DilationFilter::createOutputTypes()
{
	SelectDataStringDataAdapterBasePtr temp;

	temp = SelectDataStringDataAdapter::New();
	temp->setValueName("Output");
	temp->setHelp("Dilated segmentation image");
	mOutputTypes.push_back(temp);

	temp = SelectDataStringDataAdapter::New();
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

	// Convert radius in mm to radius in voxels for the structuring element
	Eigen::Array3d spacing = input->getSpacing();
	itk::Size<3> radiusInVoxels;
	radiusInVoxels[0] = radius/spacing(0);
	radiusInVoxels[1] = radius/spacing(1);
	radiusInVoxels[2] = radius/spacing(2);

	itkImageType::ConstPointer itkImage = AlgorithmHelper::getITKfromSSCImage(input);

	// Create structuring element
	typedef itk::BinaryBallStructuringElement<unsigned char,3> StructuringElementType;
    StructuringElementType structuringElement;
    structuringElement.SetRadius(radiusInVoxels);
    structuringElement.CreateStructuringElement();

	// Dilation
	typedef itk::BinaryDilateImageFilter<itkImageType, itkImageType, StructuringElementType> dilateFilterType;
	dilateFilterType::Pointer dilationFilter = dilateFilterType::New();
	dilationFilter->SetInput(itkImage);
	dilationFilter->SetKernel(structuringElement);
	dilationFilter->SetDilateValue(1);
	dilationFilter->Update();
	itkImage = dilationFilter->GetOutput();

	//Convert ITK to VTK
	itkToVtkFilterType::Pointer itkToVtkFilter = itkToVtkFilterType::New();
	itkToVtkFilter->SetInput(itkImage);
	itkToVtkFilter->Update();

	vtkImageDataPtr rawResult = vtkImageDataPtr::New();
	rawResult->DeepCopy(itkToVtkFilter->GetOutput());

	vtkImageCastPtr imageCast = vtkImageCastPtr::New();
	imageCast->SetInput(rawResult);
	imageCast->SetOutputScalarTypeToUnsignedChar();
	rawResult = imageCast->GetOutput();

	// TODO: possible memory problem here - check debug mem system of itk/vtk

	mRawResult =  rawResult;

	BoolDataAdapterXmlPtr generateSurface = this->getGenerateSurfaceOption(mCopiedOptions);
	if (generateSurface->getValue())
	{
        double threshold = 1;/// because the segmented image is 0..1
        mRawContour = ContourFilter::execute(mRawResult, threshold);
	}

    return true;
}

bool DilationFilter::postProcess() {
	if (!mRawResult)
		return false;

	ImagePtr input = this->getCopiedInputImage();

	if (!input)
		return false;

	QString uid = input->getUid() + "_seg%1";
	QString name = input->getName()+" seg%1";
	ImagePtr output = dataManager()->createDerivedImage(mRawResult,uid, name, input);
	mRawResult = NULL;
	if (!output)
		return false;

	output->resetTransferFunctions();
	dataManager()->loadData(output);
	dataManager()->saveImage(output, patientService()->getPatientData()->getActivePatientFolder());

	// set output
	mOutputTypes.front()->setValue(output->getUid());

	// set contour output
	if (mRawContour!=NULL)
	{
		ColorDataAdapterXmlPtr colorOption = this->getColorOption(mOptions);
		MeshPtr contour = ContourFilter::postProcess(mRawContour, output, colorOption->getValue());
		mOutputTypes[1]->setValue(contour->getUid());
		mRawContour = vtkPolyDataPtr();
	}

    return true;
}



} // namespace cx
