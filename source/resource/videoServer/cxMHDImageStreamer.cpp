#include "cxMHDImageStreamer.h"

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
	vtkMetaImageReaderPtr reader = vtkMetaImageReaderPtr::New();
	reader->SetFileName(filename.toStdString().c_str());
	reader->ReleaseDataFlagOn();
	reader->Update();

	return reader->GetOutput();
}

vtkImageDataPtr convertToTestColorImage(vtkImageDataPtr input)
{
	int N = 256;

	vtkLookupTablePtr lut = vtkLookupTablePtr::New();
	lut->SetNumberOfTableValues(N);
	lut->SetTableRange(0, N - 1);
	lut->SetSaturationRange(0, 0.5);
	lut->SetHueRange(0, 1);
	lut->SetValueRange(0, 1);
	lut->Build();
	vtkImageMapToColorsPtr mapper = vtkImageMapToColorsPtr::New();
	mapper->SetInput(input);
	mapper->SetLookupTable(lut);
	mapper->Update();
	return mapper->GetOutput();
}

MHDImageStreamer::MHDImageStreamer() :
		mInitialized(false), mSendOnce(false)
{
	setSendInterval(40);
}

QString MHDImageStreamer::getType()
{
	return "MHDFile";
}

QStringList MHDImageStreamer::getArgumentDescription()
{
	QStringList retval;
	retval << "--filename: Full name of mhd file";
	retval << "--sendonce: If true, then send just once. (true/false)";
	retval << "--secondary: If defined, two streams are sent, the secondary with a modification of the base image";
	return retval;
}


void MHDImageStreamer::initialize(StringMap arguments)
{
	ImageStreamer::initialize(arguments);
	QString filename = mArguments["filename"];

	vtkImageDataPtr source = loadImage(filename);
	if (source)
		std::cout << "MHDImageStreamer: Initialized with source file: \n\t" << mArguments["filename"].toStdString() << std::endl;
	else
	{
		std::cout << "MHDImageStreamer: Failed to initialize with source file: \n\t" << mArguments["filename"].toStdString() << std::endl;
		return;
	}

	initalizePrimaryAndSecondaryData(source);

	QString sendOnce = mArguments["sendonce"];
	mSendOnce = sendOnce.contains("true") ? true : false;
	mSendTimer = new QTimer(this);
	mSendTimer->setSingleShot(mSendOnce);
	connect(mSendTimer, SIGNAL(timeout()), this, SLOT(stream()));
	mInitialized = true;
}

bool MHDImageStreamer::startStreaming(SenderPtr sender)
{
	if (!isInitialized())
	{
		std::cout << "MHDImageStreamer: Failed to start streaming: Not initialized." << std::endl;
		return false;
	}
	mSender = sender;
	mSendTimer->start(getSendInterval());
	return true;
}

void MHDImageStreamer::stopStreaming()
{
	mSendTimer->stop();
}

void MHDImageStreamer::stream()
{
	if (!isReadyToSend())
		return;

	PackagePtr primaryPackage = this->createPackage(&mPrimaryData);
	mSender->send(primaryPackage);
	if (mSecondaryData.mImageData)
	{
		PackagePtr secondaryPackage = this->createPackage(&mSecondaryData);
		mSender->send(secondaryPackage);
	}
}

MHDImageStreamer::Data MHDImageStreamer::initializePrimaryData(vtkImageDataPtr source, QString filename)
{
	Data retval;
	QString colorFormat = "R";
	if (source->GetNumberOfScalarComponents() == 3)
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
	else if (source->GetNumberOfScalarComponents() == 4)
	{
		retval.mImageData = source;
		colorFormat = "RGBA";
	}
	else if (source->GetNumberOfScalarComponents() == 1)
	{
		retval.mImageData = source;
		colorFormat = "R";
	}

	retval.mRawUid = QString("%1 [%2]").arg(QFileInfo(filename).completeBaseName()).arg(colorFormat);
	retval.mDataSource.reset(new SplitFramesContainer(retval.mImageData));
	retval.mCurrentFrame = 0;
	return retval;
}

MHDImageStreamer::Data MHDImageStreamer::initializeSecondaryData(vtkImageDataPtr source, QString filename)
{
	Data retval;
	QString colorFormat = "R";
	if (source->GetNumberOfScalarComponents() == 3)
	{
		vtkSmartPointer<vtkImageLuminance> luminance = vtkSmartPointer<vtkImageLuminance>::New();
		luminance->SetInput(source);
		vtkImageDataPtr outData = luminance->GetOutput();
		outData->Update();
		retval.mImageData = outData;
		colorFormat = "R";
	}
	else if (source->GetNumberOfScalarComponents() == 4)
	{
		retval.mImageData = source;
		colorFormat = "RGBA";
	}
	else if (source->GetNumberOfScalarComponents() == 1)
	{
		retval.mImageData = source;
		colorFormat = "R";
	}

	retval.mRawUid = QString("uchar %1[%2]").arg(QFileInfo(filename).completeBaseName()).arg(colorFormat);
	retval.mDataSource.reset(new SplitFramesContainer(retval.mImageData));
	retval.mCurrentFrame = 0;
	return retval;
}

PackagePtr MHDImageStreamer::createPackage(Data* data)
{
	PackagePtr package(new Package());
	if (!isReadyToSend())
		return package;

	int frame = (data->mCurrentFrame++) % data->mDataSource->size();
	QString uid = data->mRawUid;
	vtkImageDataPtr copy = vtkImageDataPtr::New();
	copy->DeepCopy(data->mDataSource->get(frame));

	ssc::ImagePtr image(new ssc::Image(uid, copy));
	image->setAcquisitionTime(QDateTime::currentDateTime());

	package->mImage = image;
	return package;

}

bool MHDImageStreamer::isInitialized()
{
	return mInitialized;
//	return mSender;
//	return mSendTimer;
}

bool MHDImageStreamer::isReadyToSend()
{
	return mSender && mSender->isReady();
}

void MHDImageStreamer::initalizePrimaryAndSecondaryData(vtkImageDataPtr source)
{
	mPrimaryData = this->initializePrimaryData(source, mArguments["filename"]);
	if (mArguments.count("secondary"))
	{
		mSecondaryData = this->initializeSecondaryData(source, mArguments["filename"]);
		std::cout << "MHDImageStreamer: Initialized secondary data with uid=" << mSecondaryData.mRawUid << std::endl;
	}
}
} //namespace cx
