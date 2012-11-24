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
    return ssc::DoubleDataAdapterXml::initialize("Threshold", "",
            "Select lower threshold for the segmentation", 1, ssc::DoubleRange(0, 100, 1), 0,
            root);
}

void BinaryThresholdImageFilter::createOptions(QDomElement root)
{
    mLowerThresholdOption = this->getLowerThresholdOption(root);
    connect(mLowerThresholdOption.get(), SIGNAL(changed()), this, SLOT(thresholdSlot()));
    mOptionsAdapters.push_back(mLowerThresholdOption);
std:cout << "BinaryThresholdImageFilter::createOptions(QDomElement root) "  << mLowerThresholdOption.get() << std::endl;
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
}

void BinaryThresholdImageFilter::setActive(bool on)
{
    FilterImpl::setActive(on);

    if (!mActive)
        RepManager::getInstance()->getThresholdPreview()->removePreview();
}

void BinaryThresholdImageFilter::imageChangedSlot(QString uid)
{
  ssc::ImagePtr image = ssc::dataManager()->getImage(uid);
  if(!image)
    return;
  mLowerThresholdOption->setValueRange(ssc::DoubleRange(image->getMin(), image->getMax(), 1));
  int initValue = image->getMin() + ((image->getMax() - image->getMin()) / 10);
  mLowerThresholdOption->setValue(initValue);
  RepManager::getInstance()->getThresholdPreview()->removePreview();

//  QString imageName = image->getName();
//  if(imageName.contains("us", Qt::CaseInsensitive)) //assume the image is ultrasound
//    this->toogleSmoothingSlot(true);
}

void BinaryThresholdImageFilter::thresholdSlot()
{
//    std::cout << "BinaryThresholdImageFilter::thresholdSlot() " << mActive << std::endl;
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
}





}//namespace cx
