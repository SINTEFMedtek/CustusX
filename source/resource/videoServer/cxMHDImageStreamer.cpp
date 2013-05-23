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

DummyImageStreamer::DummyImageStreamer() :
		 mSendOnce(false)
{
	this->setSendInterval(40);
}

QString DummyImageStreamer::getType()
{
	return "MHDFile";
}

vtkImageDataPtr DummyImageStreamer::internalLoadImage(QString filename)
{
	vtkImageDataPtr source = loadImage(filename);
	if (source)
		std::cout << "DummyImageStreamer: Initialized with source file: \n\t" << getFileName().toStdString() << std::endl;
	else
		std::cout << "DummyImageStreamer: Failed to initialize with source file: \n\t" << getFileName().toStdString() << std::endl;

	return source;
}
QString DummyImageStreamer::getFileName()
{
	return mFilename;
}

void DummyImageStreamer::createTestDataSource(vtkImageDataPtr source)
{
	mPrimaryDataSource = ImageTestData::initializePrimaryData(source, getFileName());

	if (!this->shouldSetupSecondaryDataSource())
		return;

	mSecondaryDataSource = ImageTestData::initializeSecondaryData(source, getFileName());
	std::cout << "DummyImageStreamer: Initialized secondary data with uid=" << mSecondaryDataSource.mRawUid << std::endl;
}

void DummyImageStreamer::initialize(QString filename, bool secondaryStream, bool sendonce)
{
	mUseSecondaryStream = secondaryStream;
	mFilename = filename;
	vtkImageDataPtr source = this->internalLoadImage(filename);
	if (!source)
	{
		this->setInitialized(false);
		return;
	}
	this->createTestDataSource(source);
	this->setSendOnce(sendonce);
	this->createSendTimer(sendonce);
	this->setInitialized(true);
}

bool DummyImageStreamer::startStreaming(SenderPtr sender)
{
	if (!this->isInitialized())
	{
		std::cout << "DummyImageStreamer: Failed to start streaming: Not initialized." << std::endl;
		return false;
	}
	mSender = sender;
	mSendTimer->start(this->getSendInterval());
	return true;
}

void DummyImageStreamer::stopStreaming()
{
	mSendTimer->stop();
}

vtkSmartPointer<vtkImageData> DummyImageStreamer::hasSecondaryData()
{
	return mSecondaryDataSource.mImageData;
}

void DummyImageStreamer::sendTestDataFrames()
{
	PackagePtr primaryPackage = ImageTestData::createPackage(&mPrimaryDataSource);
	mSender->send(primaryPackage);

	if(!this->hasSecondaryData())
		return;

	PackagePtr secondaryPackage = ImageTestData::createPackage(&mSecondaryDataSource);
	mSender->send(secondaryPackage);
}

void DummyImageStreamer::streamSlot()
{
	if (!this->isReadyToSend())
		return;

	this->sendTestDataFrames();
}

void DummyImageStreamer::setSendOnce(bool sendonce)
{
	mSendOnce = sendonce;
}

bool DummyImageStreamer::shouldSetupSecondaryDataSource()
{
	return mUseSecondaryStream;
}

} //namespace cx
