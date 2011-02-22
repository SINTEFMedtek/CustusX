#include "cxSegmentation.h"

#include "sscDataManager.h"
#include "sscUtilHelpers.h"
#include "sscRegistrationTransform.h"

namespace cx
{
Segmentation::Segmentation() :
  TimedAlgorithm("segmenting", 10)
{
  connect(&mWatcher, SIGNAL(finished()), this, SLOT(finishedSlot()));
}

Segmentation::~Segmentation()
{}

void Segmentation::setInput(ssc::ImagePtr image, QString outputBasePath, int threshold, bool useSmoothing, double smoothSigma)
{
  mInput = image;
  mOutputBasePath = outputBasePath;
  mTheshold = threshold;
  mUseSmoothing = useSmoothing;
  mSmoothingSigma = smoothSigma;

  this->generate();
}

ssc::ImagePtr Segmentation::getOutput()
{
  return mOutput;
}

void Segmentation::finishedSlot()
{
  vtkImageDataPtr rawResult = mWatcher.future().result();

  QString uid = ssc::changeExtension(mInput->getUid(), "") + "_seg%1";
  QString name = mInput->getName()+" seg%1";
  mOutput = ssc::dataManager()->createImage(rawResult,uid, name);
  if(!mOutput)
  {
    ssc::messageManager()->sendError("Segmentation failed.");
    return;
  }

  mOutput->get_rMd_History()->setRegistration(mInput->get_rMd());
  mOutput->get_rMd_History()->addParentFrame(mInput->getUid());
  ssc::dataManager()->loadData(mOutput);
  ssc::dataManager()->saveImage(mOutput, mOutputBasePath);

  this->stopTiming();
  ssc::messageManager()->sendSuccess("Done segmenting: \"" + mOutput->getName()+"\"");

  emit finished();
}

void Segmentation::generate()
{
  this->startTiming();

  mFutureResult = QtConcurrent::run(this, &Segmentation::calculate);
  mWatcher.setFuture(mFutureResult);
}

vtkImageDataPtr Segmentation::calculate()
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
