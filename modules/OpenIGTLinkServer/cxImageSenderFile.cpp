/*
 * cxImageSenderFile.cpp
 *
 *  \date Jun 21, 2011
 *      \author christiana
 */

#include "cxImageSenderFile.h"

#include <QTimer>
#include <QDateTime>
#include <QHostAddress>
#include "igtlOSUtil.h"
#include "igtlImageMessage.h"
#include "igtlServerSocket.h"
#include "vtkImageData.h"
#include "vtkSmartPointer.h"
#include "vtkMetaImageReader.h"
#include "vtkImageImport.h"
#include "vtkLookupTable.h"
#include "vtkImageMapToColors.h"
#include "vtkMetaImageWriter.h"
#include "sscForwardDeclarations.h"

#include <vtkImageExtractComponents.h>
#include <vtkImageAppendComponents.h>

typedef vtkSmartPointer<vtkImageData> vtkImageDataPtr;
typedef vtkSmartPointer<vtkImageMapToColors> vtkImageMapToColorsPtr;
typedef vtkSmartPointer<vtkLookupTable> vtkLookupTablePtr;
typedef vtkSmartPointer<vtkMetaImageReader> vtkMetaImageReaderPtr;
typedef vtkSmartPointer<vtkImageAppendComponents> vtkImageAppendComponentsPtr;
typedef vtkSmartPointer<vtkImageExtractComponents> vtkImageExtractComponentsPtr;



namespace cx
{

void GetRandomTestMatrix(igtl::Matrix4x4& matrix);

//------------------------------------------------------------
//------------------------------------------------------------
//------------------------------------------------------------



vtkImageDataPtr loadImage(QString filename)
{
//  std::cout << "reading image " << filename.toStdString() << std::endl;
  //load the image from file
  vtkMetaImageReaderPtr reader = vtkMetaImageReaderPtr::New();
  reader->SetFileName(filename.toStdString().c_str());
  reader->ReleaseDataFlagOn();
  reader->Update();

  return reader->GetOutput();
}

//vtkImageDataPtr getTestImage()
//{
//  vtkImageImportPtr mImageImport = vtkImageImportPtr::New();
//  int W = 512;
//  int H = 512;
//
//  int numberOfComponents = 4;
////  igtl::ImageMessage::Pointer mImageMessage = igtl::ImageMessage::Pointer();
//  mImageImport->SetWholeExtent(0, W-1, 0, H-1, 0, 0);
//  mImageImport->SetDataExtent(0,W-1,0,H-1,0,0);
//  mImageImport->SetDataScalarTypeToUnsignedChar();
//  mImageImport->SetNumberOfScalarComponents(numberOfComponents);
//  mTestData.resize(W*H*numberOfComponents);
//  std::fill(mTestData.begin(), mTestData.end(), 50);
//  std::vector<unsigned char>::iterator current;
//
//  for (int y=0; y<H; ++y)
//    for (int x=0; x<W; ++x)
//    {
//      current = mTestData.begin() + int((x+W*y)*numberOfComponents);
//      current[0] = 255;
//      current[1] = 0;
//      current[2] = x/2;
//      current[3] = 0;
////      mTestData[x+W*y] = x/2;
//    }
//
//  mImageImport->SetImportVoidPointer(&(*mTestData.begin()));
//  mImageImport->Modified();
//  return mImageImport->GetOutput();
//}


vtkImageDataPtr convertToTestColorImage(vtkImageDataPtr input)
{
    int N = 256;
    //make a default system set lookuptable, grayscale...
    vtkLookupTablePtr lut = vtkLookupTablePtr::New();
    lut->SetNumberOfTableValues(N);
    //lut->SetTableRange (0, 1024); // the window of the input
    lut->SetTableRange (0, N-1); // the window of the input
    lut->SetSaturationRange (0, 0.5);
    lut->SetHueRange (0, 1);
    lut->SetValueRange (0, 1);
    lut->Build();

//    vtkDataSetMapperPtr mapper = vtkDataSetMapper::New();
//    mapper->SetInput(input);
//    mapper->SetLookupTable(lut);
//    mapper->GetOutputPort()->Print(std::cout);

    vtkImageMapToColorsPtr mapper = vtkImageMapToColorsPtr::New();
    mapper->SetInput(input);
    mapper->SetLookupTable(lut);
    mapper->Update();
    return mapper->GetOutput();
//    return mapper->GetOutputPort();
}

IGTLinkImageMessage::Pointer getVtkImageMessage(vtkImageData* image)
{
  static int staticCounter = 0;
  //------------------------------------------------------------
  // size parameters
  int   size[]     = {256, 256, 1};       // image dimension
  image->GetDimensions(size);
  size[2] = 1; // grab only one frame

  double spacingD[]  = {1.0, 1.0, 5.0};     // spacing (mm/pixel)
  float spacingF[]  = {1.0, 1.0, 5.0};     // spacing (mm/pixel)
  image->GetSpacing(spacingD);
  spacingF[0] = spacingD[0];
  spacingF[1] = spacingD[1];
  spacingF[2] = spacingD[2];
  int*   svsize   = size;
  int   svoffset[] = {0, 0, 0};           // sub-volume offset
  int   scalarType = -1;

  if (image->GetNumberOfScalarComponents()==4)
  {
    if (image->GetScalarType()==VTK_UNSIGNED_CHAR)
    {
      scalarType = igtl::ImageMessage::TYPE_UINT32;// scalar type
    }
  }

  if (image->GetNumberOfScalarComponents()==1)
  {
    if (image->GetScalarType()==VTK_UNSIGNED_SHORT)
    {
      scalarType = igtl::ImageMessage::TYPE_UINT16;// scalar type
    }
    else if (image->GetScalarType()==VTK_UNSIGNED_CHAR)
    {
      scalarType = igtl::ImageMessage::TYPE_UINT8;// scalar type
    }
  }

  if (scalarType==-1)
  {
    std::cerr << "unknown image type" << std::endl;
	return IGTLinkImageMessage::Pointer();
  }

  //------------------------------------------------------------
  // Create a new IMAGE type message
  IGTLinkImageMessage::Pointer imgMsg = IGTLinkImageMessage::New();
  imgMsg->SetDimensions(size);
  imgMsg->SetSpacing(spacingF);
  imgMsg->SetScalarType(scalarType);
  imgMsg->SetDeviceName("cxTestImage");
  imgMsg->SetSubVolume(svsize, svoffset);
  imgMsg->AllocateScalars();

  QDateTime mLastGrabTime = QDateTime::currentDateTime();
	igtl::TimeStamp::Pointer timestamp;
	timestamp = igtl::TimeStamp::New();
	//  double now = 1.0/1000*(double)QDateTime::currentDateTime().toMSecsSinceEpoch();
	double grabTime = 1.0 / 1000 * (double) mLastGrabTime.toMSecsSinceEpoch();
	timestamp->SetTime(grabTime);
	imgMsg->SetTimeStamp(timestamp);

  //------------------------------------------------------------
  // Set image data (See GetTestImage() bellow for the details)
//  GetTestImage(imgMsg, filedir, index);

  int fsize = imgMsg->GetImageSize();
  int frame = (staticCounter++) % image->GetDimensions()[2];
//  std::cout << "emitting frame " << frame << ", image size=" << fsize << ", comp="<< image->GetNumberOfScalarComponents() << ", scalarType="<< scalarType << ", dim=("<< image->GetDimensions()[0] << ", "<< image->GetDimensions()[1] << ")" << std::endl;
  memcpy(imgMsg->GetScalarPointer(), image->GetScalarPointer(0,0,frame), fsize); // not sure if we need to copy

  //------------------------------------------------------------
  // Get randome orientation matrix and set it.
  igtl::Matrix4x4 matrix;
  GetRandomTestMatrix(matrix);
  imgMsg->SetMatrix(matrix);

  return imgMsg;
}


//------------------------------------------------------------
// Function to generate random matrix.
void GetRandomTestMatrix(igtl::Matrix4x4& matrix)
{
  //float position[3];
  //float orientation[4];

  matrix[0][0] = 1.0;  matrix[1][0] = 0.0;  matrix[2][0] = 0.0; matrix[3][0] = 0.0;
  matrix[0][1] = 0.0;  matrix[1][1] = -1.0;  matrix[2][1] = 0.0; matrix[3][1] = 0.0;
  matrix[0][2] = 0.0;  matrix[1][2] = 0.0;  matrix[2][2] = 1.0; matrix[3][2] = 0.0;
  matrix[0][3] = 0.0;  matrix[1][3] = 0.0;  matrix[2][3] = 0.0; matrix[3][3] = 1.0;
}

//------------------------------------------------------------
//------------------------------------------------------------
//------------------------------------------------------------

QString MHDImageSender::getType()
{
	return "MHDFile";
}

QStringList MHDImageSender::getArgumentDescription()
{
	QStringList retval;
	retval << "--filename: Full name of mhd file";
	return retval;
}

MHDImageSender::MHDImageSender(QObject* parent) :
    ImageSender(parent),
//    mSocket(0),
    mCounter(0),
    mTimer(0)
{
}



void MHDImageSender::initialize(StringMap arguments)
{
    mCounter = 0;
    mArguments = arguments;


	QString filename = mArguments["filename"];
	mImageData = loadImage(filename);
	// mImageData = convertToTestColorImage(mImageData);

	if (mImageData->GetNumberOfScalarComponents()==3)
	{
		vtkImageAppendComponentsPtr merger = vtkImageAppendComponentsPtr::New();
		vtkImageExtractComponentsPtr splitterRGB = vtkImageExtractComponentsPtr::New();
		splitterRGB->SetInput(mImageData);
		splitterRGB->SetComponents(0, 1, 2);
		merger->SetInput(0, splitterRGB->GetOutput());

		vtkImageExtractComponentsPtr splitterA = vtkImageExtractComponentsPtr::New();
		splitterA->SetInput(mImageData);
		splitterA->SetComponents(0);
		merger->SetInput(1, splitterA->GetOutput());

		merger->Update();
		mImageData = merger->GetOutput();
	}

	if (mImageData)
	{
	    std::cout << "MHDImageSender: Initialized with source file: \n\t" << mArguments["filename"].toStdString() << std::endl;
	}
	else
	{
	    std::cout << "MHDImageSender: Failed to initialize with source file: \n\t" << mArguments["filename"].toStdString() << std::endl;
	    return;
	}

	mTimer = new QTimer(this);
	connect(mTimer, SIGNAL(timeout()), this, SLOT(tick())); // this signal will be executed in the thread of THIS, i.e. the main thread.
	//  mTimer->start(1200); // for test of the timeout feature
}

bool MHDImageSender::startStreaming(GrabberSenderPtr sender)
{
	if (!mTimer)
	{
	    std::cout << "MHDImageSender: Failed to start streaming: Not initialized." << std::endl;
	    return false;
	}
    mSender = sender;
	mTimer->start(40);
	return true;
}

void MHDImageSender::stopStreaming()
{
	mTimer->stop();
//	mSender = NULL;
}

void MHDImageSender::tick()
{
	if (mSender && mSender->isReady())
		mSender->send(getVtkImageMessage(mImageData));

//	if (!mSocket)
//		std::cout << "MHDImageSender error: no socket" << std::endl;
////  std::cout << "tick" << std::endl;
//
//  igtl::ImageMessage::Pointer imgMsg = getVtkImageMessage(mImageData);
//
//  //------------------------------------------------------------
//  // Pack (serialize) and send
//  imgMsg->Pack();
//  mSocket->write(reinterpret_cast<const char*>(imgMsg->GetPackPointer()), imgMsg->GetPackSize());
}


}
