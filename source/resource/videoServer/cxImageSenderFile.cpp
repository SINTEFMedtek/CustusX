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
#include "cxImageDataContainer.h"

#include <vtkImageExtractComponents.h>
#include <vtkImageAppendComponents.h>
#include <QFileInfo>

typedef vtkSmartPointer<vtkImageData> vtkImageDataPtr;
typedef vtkSmartPointer<vtkImageMapToColors> vtkImageMapToColorsPtr;
typedef vtkSmartPointer<vtkLookupTable> vtkLookupTablePtr;
typedef vtkSmartPointer<vtkMetaImageReader> vtkMetaImageReaderPtr;
typedef vtkSmartPointer<vtkImageAppendComponents> vtkImageAppendComponentsPtr;
typedef vtkSmartPointer<vtkImageExtractComponents> vtkImageExtractComponentsPtr;

namespace cx
{

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
    mCurrentFrame(0),
    mTimer(0)
{
}

void MHDImageSender::initialize(StringMap arguments)
{
    mArguments = arguments;

	QString filename = mArguments["filename"];
	mImageData = loadImage(filename);
	// mImageData = convertToTestColorImage(mImageData);

	QString colorFormat = "R";

	// convert from RGB to RGBA
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
		colorFormat = "RGBA";
	}
	if (mImageData->GetNumberOfScalarComponents()==4)
	{
		colorFormat = "RGBA";
	}

//	mRawUid = QString("%1-%3 [%2]").arg(QFileInfo(mArguments["filename"]).fileName()).arg(colorFormat);
	mRawUid = QString("%1 [%2]").arg(QFileInfo(mArguments["filename"]).fileName()).arg(colorFormat);

	if (mImageData)
	{
	    std::cout << "MHDImageSender: Initialized with source file: \n\t" << mArguments["filename"].toStdString() << std::endl;
	}
	else
	{
	    std::cout << "MHDImageSender: Failed to initialize with source file: \n\t" << mArguments["filename"].toStdString() << std::endl;
	    return;
	}

	mDataSource.reset(new SplitFramesContainer(mImageData));
	mCurrentFrame = 0;

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
}

void MHDImageSender::tick()
{
	if (mSender && mSender->isReady())
	{
	    int frame = (mCurrentFrame++) % mDataSource->size();
//		QString uid = mRawUid.arg(frame);
		QString uid = mRawUid;
		ssc::ImagePtr message(new ssc::Image(uid, mDataSource->get(frame)));
		message->setAcquisitionTime(QDateTime::currentDateTime());

		mSender->send(message);
	}
}


}
