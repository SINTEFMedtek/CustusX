// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.

#include "cxGEStreamerService.h"

#include <QMutexLocker>
#include "cxStringHelpers.h"

namespace cx
{


GEStreamerService::GEStreamerService()
{
}

GEStreamerService::~GEStreamerService()
{
}

QString GEStreamerService::getName()
{
	return "GE Interface";
}

std::vector<DataAdapterPtr> GEStreamerService::getSettings(QDomElement root)
{
	std::vector<DataAdapterPtr> retval;
	retval.push_back(this->getIPOption(root));
	retval.push_back(this->getStreamPortOption(root));
	retval.push_back(this->getBufferSizeOption(root));
	retval.push_back(this->getUseAutoImageSizeOption(root));
	retval.push_back(this->getImageSizeOption(root));
	retval.push_back(this->getIsotropicOption(root));
	retval.push_back(this->getTestModeOption(root));
	retval.push_back(this->getUseOpenCLOption(root));
	retval.push_back(this->getScanconvertedStreamOption(root));
	retval.push_back(this->getTissueStreamOption(root));
	retval.push_back(this->getBandwidthStreamOption(root));
	retval.push_back(this->getFrequencyStreamOption(root));
	retval.push_back(this->getVelocityStreamOption(root));

	return retval;
}

StreamerPtr GEStreamerService::createStreamer(QDomElement root)
{
	QMutexLocker lock(&mStreamerMutex);
	mStreamer.reset(new GEStreamer);
	mXmlSettings = root;
	this->sendOptions();

	return mStreamer;
}

void GEStreamerService::sendOptions()
{
	if(mStreamer)
		mStreamer->setOptions(this->generateOptions());
}

GEStreamer::Options GEStreamerService::generateOptions()
{
	GEStreamer::Options options;
	options.IP = mIPDataAdapter->getValue().toStdString();
	options.streamPort = mStreamPortDataAdapter->getValue();
	options.commandPort = -1;
	options.bufferSize = mBufferSizeDataAdapter->getValue();
	options.imageSize = mUseAutoImageSizeDataAdapter->getValue() ? -1 : this->translateToValue(mImageSizeDataAdapter->getValue());
	options.computationType = mUseAutoImageSizeDataAdapter->getValue() ? data_streaming::ANISOTROPIC : (mIsotropicDataAdapter->getValue() ? data_streaming::ANISOTROPIC : data_streaming::ISOTROPIC);
	options.testMode = mTestModeDataAdapter->getValue().toStdString();
	options.useOpenCL = mUseOpenCLDataAdapter->getValue();
	options.scanconvertedStream = mScanconvertedStreamDataAdapter->getValue();
	options.tissueStream = mTissueStreamDataAdapter->getValue();
	options.bandwidthStream = mBandwidthStreamDataAdapter->getValue();
	options.frequencyStream = mFrequencyStreamDataAdapter->getValue();
	options.velocityStream = mVelocityStreamDataAdapter->getValue();

	return options;
}

long GEStreamerService::translateToValue(QString value)
{
	long retval = 1;
	QStringList sizeList = value.split(QRegExp("[x,X,*]"), QString::SkipEmptyParts);
	for (int i = 0; i < sizeList.length(); i++)
	{
		int dimSize = convertStringWithDefault(sizeList.at(i), 1);
		retval *= dimSize;
	}
	return retval;
}

StringDataAdapterXmlPtr GEStreamerService::getIPOption(QDomElement root)
{
	if(!mIPDataAdapter)
	{
		QString defaultValue = "127.0.0.1";
		mIPDataAdapter = StringDataAdapterXml::initialize("ge_ip_scanner", "IP GE scanner", "IP of the GE scanner", defaultValue, root);
		mIPDataAdapter->setGroup("Connection");
		connect(mIPDataAdapter.get(), SIGNAL(valueWasSet()), this, SLOT(sendOptions()));
	}

	return mIPDataAdapter;
}

DoubleDataAdapterXmlPtr GEStreamerService::getStreamPortOption(QDomElement root)
{
	if(!mStreamPortDataAdapter)
	{
		mStreamPortDataAdapter = DoubleDataAdapterXml::initialize("ge_stream_port", "Stream port", "Set the stream for the GE stream port. (6534)", 6543, DoubleRange(1024, 49151, 1), 0, root);
		mStreamPortDataAdapter->setGuiRepresentation(DoubleDataAdapter::grSPINBOX);
		mStreamPortDataAdapter->setAdvanced(true);
		mStreamPortDataAdapter->setGroup("Connection");
		connect(mStreamPortDataAdapter.get(), SIGNAL(valueWasSet()), this, SLOT(sendOptions()));
	}

	return mStreamPortDataAdapter;
}

BoolDataAdapterXmlPtr	GEStreamerService::getUseAutoImageSizeOption(QDomElement root)
{
	if(!mUseAutoImageSizeDataAdapter)
	{
		bool defaultValue = true;
		mUseAutoImageSizeDataAdapter = BoolDataAdapterXml::initialize("ge_use_auto_image_size", "Automatic image size", "Setting this to true will ignore imagesize and use anisotropic voxels/pixels.", defaultValue);
		mUseAutoImageSizeDataAdapter->setAdvanced(true);
		mUseAutoImageSizeDataAdapter->setGroup("US Image");
		connect(mUseAutoImageSizeDataAdapter.get(), SIGNAL(valueWasSet()), this, SLOT(sendOptions()));
	}

	return mUseAutoImageSizeDataAdapter;
}

DoubleDataAdapterXmlPtr GEStreamerService::getBufferSizeOption(QDomElement root)
{
	if(!mBufferSizeDataAdapter)
	{
		mBufferSizeDataAdapter = DoubleDataAdapterXml::initialize("ge_buffer_size", "Buffer size", "Select the buffer size of the GE streamer in number of images.", 10, DoubleRange(0, 100, 1), 0, root);
		mBufferSizeDataAdapter->setGuiRepresentation(DoubleDataAdapter::grSPINBOX);
		mBufferSizeDataAdapter->setAdvanced(true);
		mBufferSizeDataAdapter->setGroup("Image processing");
		connect(mBufferSizeDataAdapter.get(), SIGNAL(valueWasSet()), this, SLOT(sendOptions()));
	}

	return mBufferSizeDataAdapter;
}

StringDataAdapterXmlPtr GEStreamerService::getImageSizeOption(QDomElement root)
{
	if(!mImageSizeDataAdapter)
	{
		QString defaultValue = "auto";
		mImageSizeDataAdapter = StringDataAdapterXml::initialize("ge_image_size", "Size of requested image", "Approximate number of voxels for 3D volumes or number of pixels for 2D images.", defaultValue, root);
		mImageSizeDataAdapter->setAdvanced(true);
		mImageSizeDataAdapter->setGroup("US Image");
		connect(mImageSizeDataAdapter.get(), SIGNAL(valueWasSet()), this, SLOT(sendOptions()));
	}

	return mImageSizeDataAdapter;
}

BoolDataAdapterXmlPtr	GEStreamerService::getIsotropicOption(QDomElement root)
{
	if(!mIsotropicDataAdapter)
	{
		bool defaultValue = false;
		mIsotropicDataAdapter = BoolDataAdapterXml::initialize("ge_isotropic", "Isotropic", "Force quadratic pixel or cubic voxels.", defaultValue);
		mIsotropicDataAdapter->setAdvanced(true);
		mIsotropicDataAdapter->setGroup("US Image");
		connect(mIsotropicDataAdapter.get(), SIGNAL(valueWasSet()), this, SLOT(sendOptions()));
	}

	return mIsotropicDataAdapter;
}

StringDataAdapterXmlPtr	GEStreamerService::getTestModeOption(QDomElement root)
{
	if(!mTestModeDataAdapter)
	{
		QString defaultValue = "no";
		QStringList testModes;
		testModes << defaultValue << "2D" << "3D";
		mTestModeDataAdapter = StringDataAdapterXml::initialize("ge_test_mode", "Test mode", "Test mode.", defaultValue, testModes, root);
		mTestModeDataAdapter->setAdvanced(true);
		mTestModeDataAdapter->setGroup("Debug");
		connect(mTestModeDataAdapter.get(), SIGNAL(valueWasSet()), this, SLOT(sendOptions()));
	}

	return mTestModeDataAdapter;
}

BoolDataAdapterXmlPtr	GEStreamerService::getUseOpenCLOption(QDomElement root)
{
	if(!mUseOpenCLDataAdapter)
	{
		bool defaultValue = true;
		mUseOpenCLDataAdapter = BoolDataAdapterXml::initialize("ge_use_opencl", "OpenCL scanconversion", "Scanconvert using OpenCL kernels.", defaultValue);
		mUseOpenCLDataAdapter->setAdvanced(true);
		mUseOpenCLDataAdapter->setGroup("Image processing");
		connect(mUseOpenCLDataAdapter.get(), SIGNAL(valueWasSet()), this, SLOT(sendOptions()));
	}

	return mUseOpenCLDataAdapter;
}

BoolDataAdapterXmlPtr	GEStreamerService::getScanconvertedStreamOption(QDomElement root)
{
	if(!mScanconvertedStreamDataAdapter)
	{
		bool defaultValue = true;
		mScanconvertedStreamDataAdapter = BoolDataAdapterXml::initialize("ge_get_scanconverted_stream", "B-Mode", "Request the B-Mode stream data.", defaultValue);
		mScanconvertedStreamDataAdapter->setGroup("Streams");
		connect(mScanconvertedStreamDataAdapter.get(), SIGNAL(valueWasSet()), this, SLOT(sendOptions()));
	}

	return mScanconvertedStreamDataAdapter;
}

BoolDataAdapterXmlPtr	GEStreamerService::getTissueStreamOption(QDomElement root)
{
	if(!mTissueStreamDataAdapter)
	{
		bool defaultValue = false;
		mTissueStreamDataAdapter = BoolDataAdapterXml::initialize("ge_get_tissue_stream", "Tissue", "Request the tissue stream, the doppler data is removed.", defaultValue);
		mTissueStreamDataAdapter->setGroup("Streams");
		connect(mTissueStreamDataAdapter.get(), SIGNAL(valueWasSet()), this, SLOT(sendOptions()));
	}

	return mTissueStreamDataAdapter;
}

BoolDataAdapterXmlPtr	GEStreamerService::getBandwidthStreamOption(QDomElement root)
{
	if(!mBandwidthStreamDataAdapter)
	{
		bool defaultValue = true;
		mBandwidthStreamDataAdapter = BoolDataAdapterXml::initialize("ge_get_bandwidth_stream", "Bandwidth", "Request the bandwidth stream data.", defaultValue);
		mBandwidthStreamDataAdapter->setGroup("Streams");
		connect(mBandwidthStreamDataAdapter.get(), SIGNAL(valueWasSet()), this, SLOT(sendOptions()));
	}

	return mBandwidthStreamDataAdapter;
}

BoolDataAdapterXmlPtr	GEStreamerService::getFrequencyStreamOption(QDomElement root)
{
	if(!mFrequencyStreamDataAdapter)
	{
		bool defaultValue = false;
		mFrequencyStreamDataAdapter = BoolDataAdapterXml::initialize("ge_get_frequency_stream", "Frequency", "Request the frequency stream data.", defaultValue);
		mFrequencyStreamDataAdapter->setGroup("Streams");
		connect(mFrequencyStreamDataAdapter.get(), SIGNAL(valueWasSet()), this, SLOT(sendOptions()));
	}

	return mFrequencyStreamDataAdapter;
}

BoolDataAdapterXmlPtr	GEStreamerService::getVelocityStreamOption(QDomElement root)
{
	if(!mVelocityStreamDataAdapter)
	{
		bool defaultValue = false;
		mVelocityStreamDataAdapter = BoolDataAdapterXml::initialize("ge_get_velocity_stream", "Velocity", "Request the velocity stream data.", defaultValue);
		mVelocityStreamDataAdapter->setGroup("Streams");
		connect(mVelocityStreamDataAdapter.get(), SIGNAL(valueWasSet()), this, SLOT(sendOptions()));
	}

	return mVelocityStreamDataAdapter;
}

} /* namespace cx */
