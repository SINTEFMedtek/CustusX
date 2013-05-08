#include "cxMHDImageSender.h"

#include <QTimer>
#include <QDateTime>
#include "vtkImageData.h"
#include "vtkMetaImageReader.h"
#include "vtkLookupTable.h"
#include "vtkImageMapToColors.h"
#include "sscForwardDeclarations.h"
#include "cxImageDataContainer.h"
#include "sscTypeConversions.h"

#include <vtkImageExtractComponents.h>
#include <vtkImageAppendComponents.h>
#include <vtkImageLuminance.h>
#include <QFileInfo>

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
	retval << "--secondary: If defined, two streams are sent, the secondary with a modification of the base image";
	return retval;
}

MHDImageSender::MHDImageSender(QObject* parent) :
    ImageStreamer(parent)
//    mSendTimer(0)
{
}

void MHDImageSender::initialize(StringMap arguments)
{
//    mArguments = arguments;
    ImageStreamer::initialize(arguments);

	QString filename = mArguments["filename"];
	vtkImageDataPtr source = loadImage(filename);

	if (source)
	{
	    std::cout << "MHDImageSender: Initialized with source file: \n\t" << mArguments["filename"].toStdString() << std::endl;
	}
	else
	{
	    std::cout << "MHDImageSender: Failed to initialize with source file: \n\t" << mArguments["filename"].toStdString() << std::endl;
	    return;
	}

	// mImageData = convertToTestColorImage(mImageData);

	mPrimaryData = this->initializePrimaryData(source, mArguments["filename"]);
	if (mArguments.count("secondary"))
	{
		mSecondaryData = this->initializeSecondaryData(source, mArguments["filename"]);
		std::cout << "MHDImageSender: Initialized secondary data with uid=" << mSecondaryData.mRawUid << std::endl;
	}

	mSendTimer = new QTimer(this);
	connect(mSendTimer, SIGNAL(timeout()), this, SLOT(tick())); // this signal will be executed in the thread of THIS, i.e. the main thread.
	//  mTimer->start(1200); // for test of the timeout feature
}

MHDImageSender::Data MHDImageSender::initializePrimaryData(vtkImageDataPtr source, QString filename) const
{
	Data retval;

	QString colorFormat = "R";

	// convert from RGB to RGBA
	if (source->GetNumberOfScalarComponents()==3)
	{
		vtkImageAppendComponentsPtr merger = vtkImageAppendComponentsPtr::New();
		vtkImageExtractComponentsPtr splitterRGB = vtkImageExtractComponentsPtr::New();
		splitterRGB->SetInput(source);
		splitterRGB->SetComponents(0, 1, 2);
		merger->SetInput(0, splitterRGB->GetOutput());

		vtkImageExtractComponentsPtr splitterA = vtkImageExtractComponentsPtr::New();
		splitterA->SetInput(source);
		splitterA->SetComponents(0);
		merger->SetInput(1, splitterA->GetOutput());

		merger->Update();
		retval.mImageData = merger->GetOutput();
		colorFormat = "RGBA";
	}
	else if (source->GetNumberOfScalarComponents()==4)
	{
		retval.mImageData = source;
		colorFormat = "RGBA";
	}
	else if (source->GetNumberOfScalarComponents()==1)
	{
		retval.mImageData = source;
		colorFormat = "R";
	}

//	mRawUid = QString("%1-%3 [%2]").arg(QFileInfo(mArguments["filename"]).fileName()).arg(colorFormat);
	retval.mRawUid = QString("%1 [%2]").arg(QFileInfo(filename).completeBaseName()).arg(colorFormat);

	retval.mDataSource.reset(new SplitFramesContainer(retval.mImageData));
	retval.mCurrentFrame = 0;

	return retval;
}

MHDImageSender::Data MHDImageSender::initializeSecondaryData(vtkImageDataPtr source, QString filename) const
{
	Data retval;

	QString colorFormat = "R";

	// convert from RGB to RGBA
	if (source->GetNumberOfScalarComponents()==3)
	{
		vtkSmartPointer<vtkImageLuminance> luminance = vtkSmartPointer<vtkImageLuminance>::New();
		luminance->SetInput(source);
		vtkImageDataPtr outData = luminance->GetOutput();
		outData->Update();
		retval.mImageData = outData;

		colorFormat = "R";
	}
	else if (source->GetNumberOfScalarComponents()==4)
	{
		retval.mImageData = source;
		colorFormat = "RGBA";
	}
	else if (source->GetNumberOfScalarComponents()==1)
	{
		retval.mImageData = source;
		colorFormat = "R";
	}

//	mRawUid = QString("%1-%3 [%2]").arg(QFileInfo(mArguments["filename"]).fileName()).arg(colorFormat);
	retval.mRawUid = QString("uchar %1[%2]").arg(QFileInfo(filename).completeBaseName()).arg(colorFormat);

	retval.mDataSource.reset(new SplitFramesContainer(retval.mImageData));
	retval.mCurrentFrame = 0;

	return retval;
}

bool MHDImageSender::startStreaming(GrabberSenderPtr sender)
{
	if (!mSendTimer)
	{
	    std::cout << "MHDImageSender: Failed to start streaming: Not initialized." << std::endl;
	    return false;
	}
    mSender = sender;
	mSendTimer->start(40);
	return true;
}

void MHDImageSender::stopStreaming()
{
	mSendTimer->stop();
}

void MHDImageSender::tick()
{
	if (!mSender || !mSender->isReady())
		return;

	this->send(&mPrimaryData);

	if (mSecondaryData.mImageData)
		this->send(&mSecondaryData);
}

void MHDImageSender::send(Data* data)
{
	if (!mSender || !mSender->isReady())
		return;

	int frame = (data->mCurrentFrame++) % data->mDataSource->size();
//		QString uid = mRawUid.arg(frame);
	QString uid = data->mRawUid;

	vtkImageDataPtr copy = vtkImageDataPtr::New();
	copy->DeepCopy(data->mDataSource->get(frame)); // the datasource might go out of scope - take copy

	ssc::ImagePtr message(new ssc::Image(uid, copy));
	message->setAcquisitionTime(QDateTime::currentDateTime());
	mSender->send(message);
}


}
