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
BinaryThresholdImageFilterOld::BinaryThresholdImageFilterOld() :
    ThreadedTimedAlgorithm<vtkImageDataPtr>("segmenting", 20)
{
	mUseDefaultMessages = false;
}

BinaryThresholdImageFilterOld::~BinaryThresholdImageFilterOld()
{}

void BinaryThresholdImageFilterOld::setInput(ssc::ImagePtr image, QString outputBasePath, int threshold, bool useSmoothing, double smoothSigma)
{
	mInput = image;
	mOutputBasePath = outputBasePath;
	mTheshold = threshold;
	mUseSmoothing = useSmoothing;
	mSmoothingSigma = smoothSigma;
}

ssc::ImagePtr BinaryThresholdImageFilterOld::getOutput()
{
	return mOutput;
}

void BinaryThresholdImageFilterOld::postProcessingSlot()
{
	vtkImageDataPtr rawResult = this->getResult();

	QString uid = mInput->getUid() + "_seg%1";
	QString name = mInput->getName()+" seg%1";
	mOutput = ssc::dataManager()->createDerivedImage(rawResult,uid, name, mInput);
	if(!mOutput)
	{
		ssc::messageManager()->sendError("Segmentation failed.");
		return;
	}

	mOutput->resetTransferFunctions();
	ssc::dataManager()->loadData(mOutput);
	ssc::dataManager()->saveImage(mOutput, mOutputBasePath);

	ssc::messageManager()->sendSuccess(QString("Done segmenting: \"%1\" [%2s]").arg(mOutput->getName()).arg(this->getSecondsPassedAsString()));

	//  emit finished();
}

vtkImageDataPtr BinaryThresholdImageFilterOld::calculate()
{
	itkImageType::ConstPointer itkImage = AlgorithmHelper::getITKfromSSCImage(mInput);

	//Smoothing
	if(mUseSmoothing)
	{
		typedef itk::SmoothingRecursiveGaussianImageFilter<itkImageType, itkImageType> smoothingFilterType;
		smoothingFilterType::Pointer smoohingFilter = smoothingFilterType::New();
		smoohingFilter->SetSigma(mSmoothingSigma);
		smoohingFilter->SetInput(itkImage);
		smoohingFilter->Update();
		itkImage = smoohingFilter->GetOutput();
	}

	//Binary Thresholding
	typedef itk::BinaryThresholdImageFilter<itkImageType, itkImageType> thresholdFilterType;
	thresholdFilterType::Pointer thresholdFilter = thresholdFilterType::New();
	thresholdFilter->SetInput(itkImage);
	thresholdFilter->SetOutsideValue(0);
	thresholdFilter->SetInsideValue(1);
	thresholdFilter->SetLowerThreshold(mTheshold);
	thresholdFilter->Update();
	itkImage = thresholdFilter->GetOutput();

	//Convert ITK to VTK
	itkToVtkFilterType::Pointer itkToVtkFilter = itkToVtkFilterType::New();
	itkToVtkFilter->SetInput(itkImage);
	itkToVtkFilter->Update();

	vtkImageDataPtr rawResult = vtkImageDataPtr::New();
	rawResult->DeepCopy(itkToVtkFilter->GetOutput());
	// TODO: possible memory problem here - check debug mem system of itk/vtk

	return rawResult;
}






















///--------------------------------------------------------





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

void BinaryThresholdImageFilter::createOptions(QDomElement root)
{
	mLowerThresholdOption = this->getLowerThresholdOption(root);
	connect(mLowerThresholdOption.get(), SIGNAL(changed()), this, SLOT(thresholdSlot()));
	mOptionsAdapters.push_back(mLowerThresholdOption);
	mOptionsAdapters.push_back(this->getGenerateSurfaceOption(root));
	mOptionsAdapters.push_back(this->getColorOption(root));
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
//	std::cout << "BinaryThresholdImageFilter::thresholdSlot() " << mLowerThresholdOption->getValue() << std::endl;
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

void BinaryThresholdImageFilter::postProcess()
{
	if (!mRawResult)
		return;

	ssc::ImagePtr input = this->getCopiedInputImage();

	if (!input)
		return;

	QString uid = input->getUid() + "_seg%1";
	QString name = input->getName()+" seg%1";
	ssc::ImagePtr output = ssc::dataManager()->createDerivedImage(mRawResult,uid, name, input);
	if (!output)
		return;

	output->resetTransferFunctions();
	ssc::dataManager()->loadData(output);
	ssc::dataManager()->saveImage(output, patientService()->getPatientData()->getActivePatientFolder());

	// set output
	mOutputTypes.front()->setValue(output->getUid());

	// set contour output
	ssc::ColorDataAdapterXmlPtr colorOption = this->getColorOption(mOptions);
	ssc::MeshPtr contour = ContourFilter::postProcess(mRawContour, output, colorOption->getValue());
	mOutputTypes[1]->setValue(contour->getUid());
}




/////--------------------------------------------------------


//ContouringDecoratingFilter::ContouringDecoratingFilter(FilterPtr base) :
//    mBase(base)
//{

//}


//QString ContouringDecoratingFilter::getName() const
//{
//	return mBase->getName() + "/Contouring";
//}

//QString ContouringDecoratingFilter::getType() const
//{
//	return mBase->getType() + "_Contouring";
//}

//QString ContouringDecoratingFilter::getHelp() const
//{
//	return mBase->getHelp() +
//			"<html>"
//	        "<p>In addition, check the \"Generate surface\" box to create a surface representation "
//	        "of the output.<p>"
//	        "</html>";
//}

//ssc::BoolDataAdapterXmlPtr ContouringDecoratingFilter::getGenerateSurfaceOption(QDomElement root)
//{
//	ssc::BoolDataAdapterXmlPtr retval = ssc::BoolDataAdapterXml::initialize("Generate Surface", "",
//	                                                                        "Generate a surface of the output volume", true,
//	                                                                            root);
//	return retval;
//}

//void ContouringDecoratingFilter::createOptions(QDomElement root)
//{
//	mOptionsAdapters.push_back(this->getGenerateSurfaceOption(root));
//}

//void ContouringDecoratingFilter::createInputTypes()
//{
//}

//void ContouringDecoratingFilter::createOutputTypes()
//{
//	SelectDataStringDataAdapterBasePtr temp;

//	temp = SelectDataStringDataAdapter::New();
//	temp->setValueName("Contour");
//	temp->setHelp("Output contour generated from the output volume");
//	mOutputTypes.push_back(temp);
//}

//void ContouringDecoratingFilter::setActive(bool on)
//{
//	FilterImpl::setActive(on);
//	mBase->setActive(on);
//}


//bool ContouringDecoratingFilter::preProcess()
//{
//	return FilterImpl::preProcess() && mBase->preProcess();
//}

//bool ContouringDecoratingFilter::execute()
//{
//	bool success = mBase->execute();
//	if (!success)
//		return success;

//	ContourFilter::execute()
//}

//void ContouringDecoratingFilter::postProcess()
//{
//	if (!mRawResult)
//		return;

//	ssc::ImagePtr input = this->getCopiedInputImage();

//	if (!input)
//		return;

//	QString uid = input->getUid() + "_seg%1";
//	QString name = input->getName()+" seg%1";
//	ssc::ImagePtr output = ssc::dataManager()->createDerivedImage(mRawResult,uid, name, input);
//	if (!output)
//		return;

//	output->resetTransferFunctions();
//	ssc::dataManager()->loadData(output);
//	ssc::dataManager()->saveImage(output, patientService()->getPatientData()->getActivePatientFolder());

//	// set output
//	mOutputTypes.front()->setValue(output->getUid());
//}



}//namespace cx
