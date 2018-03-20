/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxMHDImageStreamer.h"

#include <QTimer>
#include <QDateTime>
#include "vtkImageData.h"
#include "vtkMetaImageReader.h"
#include "vtkLookupTable.h"
#include "vtkImageMapToColors.h"
#include "cxForwardDeclarations.h"
#include "cxImageDataContainer.h"
#include "cxTypeConversions.h"

#include <vtkImageExtractComponents.h>
#include <vtkImageAppendComponents.h>
#include <vtkImageLuminance.h>
#include <QFileInfo>

#include "cxStringProperty.h"
#include "cxDoubleProperty.h"
#include "cxBoolProperty.h"
#include "cxDataReaderWriter.h"
#include "cxSender.h"
#include "cxFilePathProperty.h"
#include "cxProfile.h"
#include "cxReporter.h"

namespace cx
{

std::vector<PropertyPtr> ImageStreamerDummyArguments::getSettings(QDomElement root)
{
	std::vector<PropertyPtr> retval;
	retval.push_back(this->getFilenameOption(root));
	retval.push_back(this->getSecondaryOption(root));
	return retval;
}

FilePathPropertyPtr ImageStreamerDummyArguments::getFilenameOption(QDomElement root)
{
	FilePathPropertyPtr retval;
	retval = FilePathProperty::initialize("filename", "Filename",
										  "Select a 3D image file to stream from",
										  "",
										  QStringList() << profile()->getSessionRootFolder(),
										  root);

	retval->setGroup("File");
	return retval;
}

BoolPropertyBasePtr ImageStreamerDummyArguments::getSecondaryOption(QDomElement root)
{
	BoolPropertyPtr retval;
	bool defaultValue = false;
	retval = BoolProperty::initialize("secondary", "Secondary",
											"Create two streams, the second one a dummy color image",
											defaultValue, root);
	retval->setAdvanced(true);
	retval->setGroup("File");
	return retval;
}

StringMap ImageStreamerDummyArguments::convertToCommandLineArguments(QDomElement root)
{
	StringMap retval;
	retval["--type"] = "MHDFile";
	retval["--filename"] = this->getFilenameOption(root)->getValue();
	if (this->getSecondaryOption(root)->getValue())
		retval["--secondary"] = "1";
	return retval;
}

QStringList ImageStreamerDummyArguments::getArgumentDescription()
{
	QStringList retval;
	retval << "--filename:		name of image file to stream from ";
	retval << "--secondary:		Create two streams, the second one a dummy color image";
	return retval;
}

} // namespace cx


///--------------------------------------------------------
///--------------------------------------------------------
///--------------------------------------------------------

namespace cx
{

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
	mapper->SetInputData(input);
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
		splitterRGB->SetInputData(source);
		splitterRGB->SetComponents(0, 1, 2);
//		merger->AddInputConnection(0, splitterRGB->GetOutputPort());
		merger->AddInputConnection(splitterRGB->GetOutputPort());
		vtkImageExtractComponentsPtr splitterA = vtkImageExtractComponentsPtr::New();
		splitterA->SetInputData(source);
		splitterA->SetComponents(0);
		merger->AddInputConnection(splitterA->GetOutputPort());
//		merger->AddInputConnection(1, splitterA->GetOutputPort());
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
		luminance->SetInputData(source);
		luminance->Update();
		vtkImageDataPtr outData = luminance->GetOutput();
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
	ImagePtr image(new Image(uid, copy));
	image->setAcquisitionTime(QDateTime::currentDateTime());
	package->mImage = image;
	return package;
}

DummyImageStreamer::DummyImageStreamer() :
		 mSendOnce(false),
		 mUseSecondaryStream(false),
		 mPrimaryDataSource(),
		 mSecondaryDataSource()
{
	this->setSendInterval(40);
}

QString DummyImageStreamer::getType()
{
	return "MHDFile";
}

vtkImageDataPtr DummyImageStreamer::internalLoadImage(QString filename)
{
	vtkImageDataPtr source = MetaImageReader().loadVtkImageData(filename);

	if (source)
		std::cout << "DummyImageStreamer: Initialized with source file: " << getFileName().toStdString() << std::endl;
	else
		std::cout << "DummyImageStreamer: Failed to initialize with source file: " << getFileName().toStdString() << std::endl;

	return source;
}
QString DummyImageStreamer::getFileName()
{
	return mFilename;
}

QStringList DummyImageStreamer::getArgumentDescription()
{
	return ImageStreamerDummyArguments().getArgumentDescription();
}


void DummyImageStreamer::createTestDataSource(vtkImageDataPtr source)
{
	mPrimaryDataSource = ImageTestData::initializePrimaryData(source, getFileName());

	if (!this->shouldSetupSecondaryDataSource())
		return;

	mSecondaryDataSource = ImageTestData::initializeSecondaryData(source, getFileName());
	std::cout << "DummyImageStreamer: Initialized secondary data with uid=" << mSecondaryDataSource.mRawUid << std::endl;
}

void DummyImageStreamer::initialize(StringMap arguments)
{
	CommandLineStreamer::initialize(arguments);

	QString filename = arguments["filename"];
	bool secondary = arguments.count("secondary") ? true : false;
	this->initialize(filename, secondary);
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

void DummyImageStreamer::startStreaming(SenderPtr sender)
{
	if (!this->isInitialized())
	{
		reportError("DummyImageStreamer: Failed to start streaming: Not initialized.");
		return;
	}
	mSender = sender;
	mSendTimer->start(this->getSendInterval());
}

void DummyImageStreamer::stopStreaming()
{
	mSendTimer->stop();
}

bool DummyImageStreamer::isStreaming()
{
	return this->isInitialized();
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
