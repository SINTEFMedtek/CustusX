#include "cxTimedAlgorithm.h"

#include "itkImageFileReader.h"
#include "sscTypeConversions.h"
#include "vtkMetaImageWriter.h"
#include "cxSettings.h"
#include "sscUtilHelpers.h"

namespace cx
{
//---------------------------------------------------------------------------------------------------------------------
itkImageType::ConstPointer AlgorithmHelper::getITKfromSSCImage(ssc::ImagePtr image)
{
  return AlgorithmHelper::getITKfromSSCImageViaFile(image);
//  if(!image)
//  {
//    std::cout << "getITKfromSSCImage(): NO image!!!" << std::endl;
//    return itkImageType::ConstPointer();
//  }
//  itkVTKImageToImageFilterType::Pointer vtk2itkFilter = itkVTKImageToImageFilterType::New();
//  //itkToVtkFilter->SetInput(data);
//  vtkImageDataPtr input = image->getBaseVtkImageData();
//  if (input->GetScalarType() != VTK_UNSIGNED_SHORT)
//  //if (input->GetScalarType() == VTK_UNSIGNED_CHAR)
//  {
//    // convert
//    // May need to use vtkImageShiftScale instead if we got data types other than unsigned char?
//    vtkImageCastPtr imageCast = vtkImageCastPtr::New();
//    imageCast->SetInput(input);
//    imageCast->SetOutputScalarTypeToUnsignedShort();
//    input = imageCast->GetOutput();
//  }
//  vtk2itkFilter->SetInput(input);
//  vtk2itkFilter->Update();
//  return vtk2itkFilter->GetOutput();
}
//---------------------------------------------------------------------------------------------------------------------


/**This is a workaround for _unpredictable_ crashes
 * experienced when using the itk::VTKImageToImageFilter.
 *
 */
itkImageType::ConstPointer AlgorithmHelper::getITKfromSSCImageViaFile(ssc::ImagePtr image)
{
  if(!image)
  {
    std::cout << "getITKfromSSCImage(): NO image!!!" << std::endl;
    return itkImageType::ConstPointer();
  }

  QString tempFolder = settings()->value("globalPatientDataFolder").toString() + "/NoPatient/temp/";
  QDir().mkpath(tempFolder);

  // write to disk
  vtkMetaImageWriterPtr writer = vtkMetaImageWriterPtr::New();

  QString filename = tempFolder + "/"+qstring_cast(writer.GetPointer())+".mhd";


  writer->SetInput(image->getBaseVtkImageData());
  writer->SetFileName(cstring_cast(filename));
  writer->SetCompression(false);
  writer->Write();

  // read from disk
  typedef itk::ImageFileReader<itkImageType> ReaderType;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName(cstring_cast(filename));
  reader->Update();
  itkImageType::ConstPointer retval = reader->GetOutput();

  QFile(filename).remove(); // cleanup
  QFile(ssc::changeExtension(filename, "raw")).remove(); // cleanup

  return retval;
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
//---------------------------------------------------------------------------------------------------------------------
template <class T>
ThreadedTimedAlgorithm<T>::ThreadedTimedAlgorithm(QString product, int secondsBetweenAnnounce) :
  TimedAlgorithm(product, secondsBetweenAnnounce)
{
  connect(&mWatcher, SIGNAL(finished()), this, SLOT(finishedSlot()));
  connect(&mWatcher, SIGNAL(finished()), this, SLOT(postProcessingSlot()));
}

template <class T>
ThreadedTimedAlgorithm<T>::~ThreadedTimedAlgorithm()
{}

template <class T>
void ThreadedTimedAlgorithm<T>::finishedSlot()
{
  TimedAlgorithm::stopTiming();
}

template <class T>
void ThreadedTimedAlgorithm<T>::generate()
{
  TimedAlgorithm::startTiming();

  mFutureResult = QtConcurrent::run(this, &ThreadedTimedAlgorithm<T>::calculate);
  mWatcher.setFuture(mFutureResult);
}
template <class T>
T ThreadedTimedAlgorithm<T>::getResult()
{
  T result = mWatcher.future().result();
  return result;
}


template class ThreadedTimedAlgorithm<vtkImageDataPtr>; //centerline
template class ThreadedTimedAlgorithm<ssc::ImagePtr>; //resample
//template class ThreadedTimedAlgorithm<vtkImageDataPtr>; //segmentation
template class ThreadedTimedAlgorithm<vtkPolyDataPtr>; //contour


//---------------------------------------------------------------------------------------------------------------------
Example::Example() :
    ThreadedTimedAlgorithm<QString>("TestProduct", 1)
{
  std::cout << "Test::Test()" << std::endl;
  this->generate();
}

Example::~Example()
{}

void Example::postProcessingSlot()
{
  QString result = this->getResult();
  std::cout << "void Test::postProcessingSlot(), result: "<< result.toStdString() << std::endl;
}

QString Example::calculate()
{
  std::cout << " QString Test::calculate()" << std::endl;

#ifdef WIN32
  Sleep(500);
#else
  sleep(5);
#endif

  return QString("Test successful!!!");
}
}//namespace cx

