#include "cxTimedAlgorithm.h"

namespace cx
{
//---------------------------------------------------------------------------------------------------------------------
itkImageType::ConstPointer AlgorithmHelper::getITKfromSSCImage(ssc::ImagePtr image)
{
  if(!image)
  {
    std::cout << "getITKfromSSCImage(): NO image!!!" << std::endl;
    return itkImageType::ConstPointer();
  }
  itkVTKImageToImageFilterType::Pointer vtk2itkFilter = itkVTKImageToImageFilterType::New();
  //itkToVtkFilter->SetInput(data);
  vtkImageDataPtr input = image->getBaseVtkImageData();
  if (input->GetScalarType() != VTK_UNSIGNED_SHORT)
  //if (input->GetScalarType() == VTK_UNSIGNED_CHAR)
  {
    // convert
    // May need to use vtkImageShiftScale instead if we got data types other than unsigned char?
    vtkImageCastPtr imageCast = vtkImageCastPtr::New();
    imageCast->SetInput(input);
    imageCast->SetOutputScalarTypeToUnsignedShort();
    input = imageCast->GetOutput();
  }
  vtk2itkFilter->SetInput(input);
  vtk2itkFilter->Update();
  return vtk2itkFilter->GetOutput();
}
//---------------------------------------------------------------------------------------------------------------------

TimedAlgorithm::TimedAlgorithm(QString product, int secondsBetweenAnnounce) :
    QObject(),
    mProduct(product)
{
  mTimer = new QTimer(this);
  connect(mTimer, SIGNAL(timeout()), this, SLOT(timeoutSlot()));
  mTimer->setInterval(secondsBetweenAnnounce*1000);
}

TimedAlgorithm::~TimedAlgorithm()
{}

void TimedAlgorithm::startTiming()
{
  mStartTime = QDateTime::currentDateTime();
  mTimer->start();
}

void TimedAlgorithm::stopTiming()
{
  QTime timePassed = this->getTimePassed();
  ssc::messageManager()->sendInfo("Generating took: " + timePassed.toString("hh:mm:ss:zzz"));

  mStartTime = QDateTime();
  mTimer->stop();
}

QTime TimedAlgorithm::getTimePassed()
{
  QTime retval = QTime(0, 0);
  retval = retval.addMSecs(mStartTime.time().msecsTo(QDateTime::currentDateTime().time()));
  return retval;
}

void TimedAlgorithm::timeoutSlot()
{
  ssc::messageManager()->sendInfo("Still working on generating the "+mProduct+", "+this->getTimePassed().toString()+" has passed.");
}
}//namespace cx

