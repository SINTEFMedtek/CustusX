#include "cxBinaryThresholdImageFilter.h"

#include "cxAlgorithmHelpers.h"

#include "sscDataManager.h"
#include "sscUtilHelpers.h"
#include "sscRegistrationTransform.h"

namespace cx
{
BinaryThresholdImageFilter::BinaryThresholdImageFilter() :
    ThreadedTimedAlgorithm<vtkImageDataPtr>("segmenting", 20)
{
	mUseDefaultMessages = false;
}

BinaryThresholdImageFilter::~BinaryThresholdImageFilter()
{}

void BinaryThresholdImageFilter::setInput(ssc::ImagePtr image, QString outputBasePath, int threshold, bool useSmoothing, double smoothSigma)
{
  mInput = image;
  mOutputBasePath = outputBasePath;
  mTheshold = threshold;
  mUseSmoothing = useSmoothing;
  mSmoothingSigma = smoothSigma;
}

ssc::ImagePtr BinaryThresholdImageFilter::getOutput()
{
  return mOutput;
}

void BinaryThresholdImageFilter::postProcessingSlot()
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

vtkImageDataPtr BinaryThresholdImageFilter::calculate()
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


}//namespace cx
