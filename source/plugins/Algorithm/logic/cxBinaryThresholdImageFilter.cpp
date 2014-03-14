#include "cxBinaryThresholdImageFilter.h"

#include "cxAlgorithmHelpers.h"
#include <itkBinaryThresholdImageFilter.h>
#include <vtkImageCast.h>
#include "cxDataManager.h"
#include "cxUtilHelpers.h"
#include "cxRegistrationTransform.h"
#include "cxStringDataAdapterXml.h"
#include "cxDoubleDataAdapterXml.h"
#include "cxColorDataAdapterXml.h"
#include "cxBoolDataAdapterXml.h"
#include "cxTypeConversions.h"

#include "cxPatientService.h"
#include "cxPatientData.h"
#include "cxRepManager.h"
#include "cxThresholdPreview.h"
#include "cxContourFilter.h"
#include "cxMesh.h"
#include "cxImage.h"
#include "cxSelectDataStringDataAdapter.h"


namespace cx
{

QString BinaryThresholdImageFilter::getName() const
{
	return "Segmentation";
}

QString BinaryThresholdImageFilter::getType() const
{
	return "BinaryThresholdImageFilter";
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

DoubleDataAdapterXmlPtr BinaryThresholdImageFilter::getLowerThresholdOption(QDomElement root)
{
	DoubleDataAdapterXmlPtr retval = DoubleDataAdapterXml::initialize("Threshold", "",
	                                                                            "Select lower threshold for the segmentation", 1, DoubleRange(0, 100, 1), 0,
	                                                                            root);
	retval->setAddSlider(true);
	return retval;
}

BoolDataAdapterXmlPtr BinaryThresholdImageFilter::getGenerateSurfaceOption(QDomElement root)
{
	BoolDataAdapterXmlPtr retval = BoolDataAdapterXml::initialize("Generate Surface", "",
	                                                                        "Generate a surface of the output volume", true,
	                                                                            root);
	return retval;
}

ColorDataAdapterXmlPtr BinaryThresholdImageFilter::getColorOption(QDomElement root)
{
	return ColorDataAdapterXml::initialize("Color", "",
	                                            "Color of output model.",
	                                            QColor("green"), root);
}

void BinaryThresholdImageFilter::createOptions()
{
	mLowerThresholdOption = this->getLowerThresholdOption(mOptions);
	connect(mLowerThresholdOption.get(), SIGNAL(changed()), this, SLOT(thresholdSlot()));
	mOptionsAdapters.push_back(mLowerThresholdOption);
	mOptionsAdapters.push_back(this->getGenerateSurfaceOption(mOptions));
	mOptionsAdapters.push_back(this->getColorOption(mOptions));
}

void BinaryThresholdImageFilter::createInputTypes()
{
	SelectDataStringDataAdapterBasePtr temp;

	temp = SelectImageStringDataAdapter::New();
	temp->setValueName("Input");
	temp->setHelp("Select image input for thresholding");
	connect(temp.get(), SIGNAL(dataChanged(QString)), this, SLOT(imageChangedSlot(QString)));
	mInputTypes.push_back(temp);
}

void BinaryThresholdImageFilter::createOutputTypes()
{
	SelectDataStringDataAdapterBasePtr temp;

	temp = SelectDataStringDataAdapter::New();
	temp->setValueName("Output");
	temp->setHelp("Output thresholded binary image");
	mOutputTypes.push_back(temp);

	temp = SelectDataStringDataAdapter::New();
	temp->setValueName("Contour");
	temp->setHelp("Output contour generated from thresholded binary image.");
	mOutputTypes.push_back(temp);
}

void BinaryThresholdImageFilter::setActive(bool on)
{
	FilterImpl::setActive(on);

	if (!mActive)
		RepManager::getInstance()->getThresholdPreview()->removePreview();
}

void BinaryThresholdImageFilter::imageChangedSlot(QString uid)
{
	this->updateThresholdFromImageChange(uid, mLowerThresholdOption);
	RepManager::getInstance()->getThresholdPreview()->removePreview();
}

void BinaryThresholdImageFilter::thresholdSlot()
{
	if (mActive)
	{
		ImagePtr image = boost::dynamic_pointer_cast<Image>(mInputTypes[0]->getData());
		RepManager::getInstance()->getThresholdPreview()->setPreview(image,
		                                                             mLowerThresholdOption->getValue());
	}
}

bool BinaryThresholdImageFilter::preProcess()
{
	RepManager::getInstance()->getThresholdPreview()->removePreview();
	return FilterImpl::preProcess();

}

bool BinaryThresholdImageFilter::execute()
{
	ImagePtr input = this->getCopiedInputImage();
	if (!input)
		return false;

	DoubleDataAdapterXmlPtr lowerThreshold = this->getLowerThresholdOption(mCopiedOptions);
	BoolDataAdapterXmlPtr generateSurface = this->getGenerateSurfaceOption(mCopiedOptions);

	itkImageType::ConstPointer itkImage = AlgorithmHelper::getITKfromSSCImage(input);

	//Binary Thresholding
	typedef itk::BinaryThresholdImageFilter<itkImageType, itkImageType> thresholdFilterType;
	thresholdFilterType::Pointer thresholdFilter = thresholdFilterType::New();
	thresholdFilter->SetInput(itkImage);
	thresholdFilter->SetOutsideValue(0);
	thresholdFilter->SetInsideValue(1);
	thresholdFilter->SetLowerThreshold(lowerThreshold->getValue());
	thresholdFilter->Update();
	itkImage = thresholdFilter->GetOutput();

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


}//namespace cx
