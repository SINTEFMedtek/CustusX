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

vtkLookupTablePtr createLookupTable(int numberOfTableValues)
{
	vtkLookupTablePtr lut = vtkLookupTablePtr::New();
	lut->SetNumberOfTableValues(numberOfTableValues);
	lut->SetTableRange(0, numberOfTableValues - 1);
	lut->SetSaturationRange(0, 0.5);
	lut->SetHueRange(0, 1);
	lut->SetValueRange(0, 1);
	lut->Build();
	return lut;
}

vtkImageDataPtr applyLUTToImage(vtkImageDataPtr input, vtkLookupTablePtr lut)
{
	vtkImageMapToColorsPtr mapper = vtkImageMapToColorsPtr::New();
	mapper->SetInput(input);
	mapper->SetLookupTable(lut);
	mapper->Update();
	vtkImageDataPtr retval = mapper->GetOutput();
	return retval;
}

vtkImageDataPtr convertToTestColorImage(vtkImageDataPtr image)
{
	int numberOfTableValues = 256;
	vtkLookupTablePtr lut = createLookupTable(numberOfTableValues);
	vtkImageDataPtr retval = applyLUTToImage(image, lut);
	return retval;
}


ImageTestData ImageTestData::initializePrimaryData(vtkImageDataPtr source, QString filename)
{
	ImageTestData retval;
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

ImageTestData ImageTestData::initializeSecondaryData(vtkImageDataPtr source, QString filename)
{
	ImageTestData retval;
	QString colorFormat = "R";
	if (source->GetNumberOfScalarComponents() == 3)
	{
		vtkSmartPointer < vtkImageLuminance > luminance = vtkSmartPointer < vtkImageLuminance > ::New();
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

PackagePtr ImageTestData::createPackage(ImageTestData* data)
{
	PackagePtr package(new Package());

	int frame = (data->mCurrentFrame++) % data->mDataSource->size();
	QString uid = data->mRawUid;
	vtkImageDataPtr copy = vtkImageDataPtr::New();
	copy->DeepCopy(data->mDataSource->get(frame));
	ssc::ImagePtr image(new ssc::Image(uid, copy));
	image->setAcquisitionTime(QDateTime::currentDateTime());
	package->mImage = image;
	return package;
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


void MHDImageStreamer::createSendTimer()
{
	mSendTimer = new QTimer(this);
	mSendTimer->setSingleShot(mSendOnce);
	connect(mSendTimer, SIGNAL(timeout()), this, SLOT(stream()));
}

vtkImageDataPtr MHDImageStreamer::internalLoadImage(QString filename)
{
	vtkImageDataPtr source = loadImage(filename);
	if (source)
		std::cout << "MHDImageStreamer: Initialized with source file: \n\t" << getFileName().toStdString() << std::endl;
	else
		std::cout << "MHDImageStreamer: Failed to initialize with source file: \n\t" << getFileName().toStdString() << std::endl;

	return source;
}
QString MHDImageStreamer::getFileName()
{
	return mArguments["filename"];
}

void MHDImageStreamer::createTestDataSource(vtkImageDataPtr source)
{
	initalizePrimaryAndSecondaryDataSource(source);
}

void MHDImageStreamer::setInitialized(bool initialized)
{
	mInitialized = initialized;
}

void MHDImageStreamer::initialize(StringMap arguments)
{
	ImageStreamer::initialize(arguments);
	QString filename = getFileName();
	vtkImageDataPtr source = internalLoadImage(filename);
	if (!source)
	{
		setInitialized(false);
		return;
	}
	createTestDataSource(source);
	setSendOnce();
	createSendTimer();
	setInitialized(true);
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

vtkSmartPointer<vtkImageData> MHDImageStreamer::hasSecondaryData()
{
	return mSecondaryDataSource.mImageData;
}

void MHDImageStreamer::sendTestDataFrames()
{
	PackagePtr primaryPackage = ImageTestData::createPackage(&mPrimaryDataSource);
	mSender->send(primaryPackage);

	if(!hasSecondaryData())
		return;

	PackagePtr secondaryPackage = ImageTestData::createPackage(&mSecondaryDataSource);
	mSender->send(secondaryPackage);
}

void MHDImageStreamer::stream()
{
	if (!isReadyToSend())
		return;

	sendTestDataFrames();
}

bool MHDImageStreamer::isInitialized()
{
	return mInitialized;
}

bool MHDImageStreamer::isReadyToSend()
{
	return mSender && mSender->isReady();
}

void MHDImageStreamer::initalizePrimaryAndSecondaryDataSource(vtkImageDataPtr source)
{
	mPrimaryDataSource = ImageTestData::initializePrimaryData(source, getFileName());

	if (!shouldSetupSecondaryDataSource())
		return;

	mSecondaryDataSource = ImageTestData::initializeSecondaryData(source, getFileName());
	std::cout << "MHDImageStreamer: Initialized secondary data with uid=" << mSecondaryDataSource.mRawUid << std::endl;
}

void MHDImageStreamer::setSendOnce()
{
	mSendOnce = mArguments["sendonce"].contains("true") ? true : false;
}

unsigned long int MHDImageStreamer::shouldSetupSecondaryDataSource()
{
	return mArguments.count("secondary");
}

} //namespace cx
