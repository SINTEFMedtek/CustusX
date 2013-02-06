#include "cxBinaryThresholdImageFilter.h"

#include "cxAlgorithmHelpers.h"

#include "sscDataManager.h"
#include "sscUtilHelpers.h"
#include "sscRegistrationTransform.h"
#include "sscStringDataAdapterXml.h"
#include "sscDoubleDataAdapterXml.h"
#include "sscBoolDataAdapterXml.h"
#include "sscTypeConversions.h"

#include "cxPatientService.h"
#include "cxPatientData.h"
#include "cxRepManager.h"
#include "cxThresholdPreview.h"
#include "cxContourFilter.h"
#include "sscMesh.h"

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

ssc::DoubleDataAdapterXmlPtr BinaryThresholdImageFilter::getLowerThresholdOption(QDomElement root)
{
	ssc::DoubleDataAdapterXmlPtr retval = ssc::DoubleDataAdapterXml::initialize("Threshold", "",
	                                                                            "Select lower threshold for the segmentation", 1, ssc::DoubleRange(0, 100, 1), 0,
	                                                                            root);
	retval->setAddSlider(true);
	return retval;
}

ssc::BoolDataAdapterXmlPtr BinaryThresholdImageFilter::getGenerateSurfaceOption(QDomElement root)
{
	ssc::BoolDataAdapterXmlPtr retval = ssc::BoolDataAdapterXml::initialize("Generate Surface", "",
	                                                                        "Generate a surface of the output volume", true,
	                                                                            root);
	return retval;
}

ssc::ColorDataAdapterXmlPtr BinaryThresholdImageFilter::getColorOption(QDomElement root)
{
	return ssc::ColorDataAdapterXml::initialize("Color", "",
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
		ssc::ImagePtr image = boost::shared_dynamic_cast<ssc::Image>(mInputTypes[0]->getData());
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
	ssc::ImagePtr input = this->getCopiedInputImage();
	if (!input)
		return false;

	ssc::DoubleDataAdapterXmlPtr lowerThreshold = this->getLowerThresholdOption(mCopiedOptions);

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
	// TODO: possible memory problem here - check debug mem system of itk/vtk

	mRawResult =  rawResult;

    double threshold = 1;/// because the segmented image is 0..1
	mRawContour = ContourFilter::execute(mRawResult, threshold);

	return true;
}

bool BinaryThresholdImageFilter::postProcess()
{
	if (!mRawResult)
		return false;

	ssc::ImagePtr input = this->getCopiedInputImage();

	if (!input)
		return false;

	QString uid = input->getUid() + "_seg%1";
	QString name = input->getName()+" seg%1";
	ssc::ImagePtr output = ssc::dataManager()->createDerivedImage(mRawResult,uid, name, input);
	if (!output)
		return false;

	output->resetTransferFunctions();
	ssc::dataManager()->loadData(output);
	ssc::dataManager()->saveImage(output, patientService()->getPatientData()->getActivePatientFolder());

	// set output
	mOutputTypes.front()->setValue(output->getUid());

	// set contour output
	ssc::ColorDataAdapterXmlPtr colorOption = this->getColorOption(mOptions);
	ssc::MeshPtr contour = ContourFilter::postProcess(mRawContour, output, colorOption->getValue());
	mOutputTypes[1]->setValue(contour->getUid());

	return true;
}


}//namespace cx
