#include "cxCenterline.h"

#include "sscMessageManager.h"
#include "sscDataManager.h"
#include "sscRegistrationTransform.h"
#include "sscUtilHelpers.h"

namespace cx
{
Centerline::Centerline() :
    ThreadedTimedAlgorithm<vtkImageDataPtr>("centerline", 10)
{}

Centerline::~Centerline()
{}

void Centerline::setInput(ssc::ImagePtr inputImage, QString outputBasePath)
{
  mInput = inputImage;
  mOutputBasePath = outputBasePath;

  this->generate();
}

ssc::ImagePtr Centerline::getOutput()
{
  return mOutput;
}

void Centerline::postProcessingSlot()
{
  vtkImageDataPtr rawResult = this->getResult();
  if(!rawResult)
  {
    ssc::messageManager()->sendError("Centerline extraction failed.");
    return;
  }

  QString uid = ssc::changeExtension(mInput->getUid(), "") + "_cl%1";
  QString name = mInput->getName()+" cl%1";
  mOutput = ssc::dataManager()->createImage(rawResult,uid, name);

  mOutput->get_rMd_History()->setRegistration(mInput->get_rMd());
  mOutput->get_rMd_History()->addParentFrame(mInput->getUid());
  ssc::dataManager()->loadData(mOutput);
  ssc::dataManager()->saveImage(mOutput, mOutputBasePath);

  ssc::messageManager()->sendSuccess("Created centerline \"" + mOutput->getName()+"\"");

  emit finished();
}

vtkImageDataPtr Centerline::calculate()
{
  ssc::messageManager()->sendInfo("Generating \""+mInput->getName()+"\" centerline... Please wait!");

  itkImageType::ConstPointer itkImage = AlgorithmHelper::getITKfromSSCImage(mInput);

  //Centerline extraction
  typedef itk::BinaryThinningImageFilter3D<itkImageType, itkImageType> centerlineFilterType;
  centerlineFilterType::Pointer centerlineFilter = centerlineFilterType::New();
  centerlineFilter->SetInput(itkImage);
  centerlineFilter->Update();
  itkImage = centerlineFilter->GetOutput();

  //Convert ITK to VTK
  itkToVtkFilterType::Pointer itkToVtkFilter = itkToVtkFilterType::New();
  itkToVtkFilter->SetInput(itkImage);
  itkToVtkFilter->Update();

  vtkImageDataPtr rawResult = vtkImageDataPtr::New();
  rawResult->DeepCopy(itkToVtkFilter->GetOutput());

  return rawResult;
}
}//namespace cx
