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
	return "GEStreamerService";
}

std::vector<DataAdapterPtr> GEStreamerService::getSettings(QDomElement root)
{
	std::vector<DataAdapterPtr> retval;
	retval.push_back(this->getIPOption(root));
	retval.push_back(this->getStreamPortOption(root));
	retval.push_back(this->getCommandPortOption(root));
	retval.push_back(this->getBufferSizeOption(root));
	retval.push_back(this->getImageSizeOption(root));
	retval.push_back(this->getIsotropicOption(root));
	retval.push_back(this->getKernelPathOption(root));
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
	//TODO
	return StreamerPtr();
}

StringDataAdapterXmlPtr GEStreamerService::getIPOption(QDomElement root)
{
	if(!mSelectIPDataAdapter)
	{
		QString defaultValue = "127.0.0.1";
		mSelectIPDataAdapter = StringDataAdapterXml::initialize("ge_ip_scanner", "IP GE scanner", "IP of the GE scanner", defaultValue, root);
//		connect(mBufferSizeDataAdapter.get(), SIGNAL(valueWasSet()), this, SLOT(updateBufferSize()));
//		this->updateBufferSize();
	}

	return mSelectIPDataAdapter;
}

DoubleDataAdapterXmlPtr GEStreamerService::getStreamPortOption(QDomElement root)
{
	if(!mStreamPortDataAdapter)
	{
		//TODO -1????
		mStreamPortDataAdapter = DoubleDataAdapterXml::initialize("ge_stream_port", "Stream port", "Set the stream for the GE stream port.", 6543, DoubleRange(1024, 49151, 1), 0, root);
		mStreamPortDataAdapter->setGuiRepresentation(DoubleDataAdapter::grSPINBOX);
//		connect(mStreamPortDataAdapter.get(), SIGNAL(valueWasSet()), this, SLOT(updateStreamPort()));
//		this->updateStreamPort();
	}

	return mStreamPortDataAdapter;
}

DoubleDataAdapterXmlPtr GEStreamerService::getCommandPortOption(QDomElement root)
{
	if(!mCommandPortDataAdapter)
	{
		//TODO -1????
		mCommandPortDataAdapter = DoubleDataAdapterXml::initialize("ge_command_port", "Command port", "Set the command for the GE stream port.", 6543, DoubleRange(1024, 49151, 1), 0, root);
		mCommandPortDataAdapter->setGuiRepresentation(DoubleDataAdapter::grSPINBOX);
//		connect(mCommandPortDataAdapter.get(), SIGNAL(valueWasSet()), this, SLOT(updateCommandPort()));
//		this->updateCommandPort();
	}

	return mCommandPortDataAdapter;
}

DoubleDataAdapterXmlPtr GEStreamerService::getBufferSizeOption(QDomElement root)
{
	if(!mBufferSizeDataAdapter)
	{
		mBufferSizeDataAdapter = DoubleDataAdapterXml::initialize("ge_buffer_size", "Buffer size", "Select the buffer size of the GE streamer in number of images.", 10, DoubleRange(0, 100, 1), 0, root);
		mBufferSizeDataAdapter->setGuiRepresentation(DoubleDataAdapter::grSPINBOX);
//		connect(mBufferSizeDataAdapter.get(), SIGNAL(valueWasSet()), this, SLOT(updateBufferSize()));
//		this->updateBufferSize();
	}

	return mBufferSizeDataAdapter;
}

StringDataAdapterXmlPtr GEStreamerService::getImageSizeOption(QDomElement root)
{
	if(!mImageSizeDataAdapter)
	{
		QString defaultValue = "auto";
		mImageSizeDataAdapter = StringDataAdapterXml::initialize("ge_image_size", "Size of requested image", "Approximate number of voxels for 3D volumes or number of pixels for 2D images.", defaultValue, root);
//		connect(mBufferSizeDataAdapter.get(), SIGNAL(valueWasSet()), this, SLOT(updateBufferSize()));
//		this->updateBufferSize();
	}

	return mImageSizeDataAdapter;
}

BoolDataAdapterXmlPtr	GEStreamerService::getIsotropicOption(QDomElement root)
{
	if(!mIsotropicDataAdapter)
	{
		bool defaultValue = false;
		mIsotropicDataAdapter = BoolDataAdapterXml::initialize("ge_isotropic", "Use isotropic", "Force quadratic pixel or cubic voxels.", defaultValue);
//		connect(mBufferSizeDataAdapter.get(), SIGNAL(valueWasSet()), this, SLOT(updateBufferSize()));
//		this->updateBufferSize();
	}

	return mIsotropicDataAdapter;
}

//DEPRECATED
//StringDataAdapterXmlPtr GEStreamerService::getKernelPathOption(QDomElement root)
//{
//	if(!mKernelPathDataAdapter)
//	{
//		QString defaultValue = "/kernel/path/";
//		mKernelPathDataAdapter = StringDataAdapterXml::initialize("ge_kernel_path", "Path to OpenCL scanconversion", "Path to kernel code for scanconversion.", defaultValue, root);
////		connect(mBufferSizeDataAdapter.get(), SIGNAL(valueWasSet()), this, SLOT(updateBufferSize()));
////		this->updateBufferSize();
//	}
//
//	return mKernelPathDataAdapter;
//}

StringDataAdapterXmlPtr	GEStreamerService::getTestModeOption(QDomElement root)
{
	if(!mTestModeDataAdapter)
	{
		QString defaultValue = "no";
		QStringList testModes;
		testModes << defaultValue << "2D" << "3D";
		mTestModeDataAdapter = StringDataAdapterXml::initialize("ge_test_mode", "Test mode", "Test mode.", defaultValue, testModes, root);
//		connect(mBufferSizeDataAdapter.get(), SIGNAL(valueWasSet()), this, SLOT(updateBufferSize()));
//		this->updateBufferSize();
	}

	return mTestModeDataAdapter;
}

BoolDataAdapterXmlPtr	GEStreamerService::getUseOpenCLOption(QDomElement root)
{
	if(!mUseOpenCLDataAdapter)
	{
		bool defaultValue = true;
		mUseOpenCLDataAdapter = BoolDataAdapterXml::initialize("ge_use_opencl", "Use OpenCL scanconversion", "Scanconvert using OpenCL kernels.", defaultValue);
//		connect(mBufferSizeDataAdapter.get(), SIGNAL(valueWasSet()), this, SLOT(updateBufferSize()));
//		this->updateBufferSize();
	}

	return mUseOpenCLDataAdapter;
}

BoolDataAdapterXmlPtr	GEStreamerService::getScanconvertedStreamOption(QDomElement root)
{
	if(!mScanconvertedStreamDataAdapter)
	{
		bool defaultValue = true;
		mScanconvertedStreamDataAdapter = BoolDataAdapterXml::initialize("ge_get_scanconverted_stream", "B-Mode stream", "Request the B-Mode stream data.", defaultValue);
//		connect(mBufferSizeDataAdapter.get(), SIGNAL(valueWasSet()), this, SLOT(updateBufferSize()));
//		this->updateBufferSize();
	}

	return mScanconvertedStreamDataAdapter;
}

BoolDataAdapterXmlPtr	GEStreamerService::getTissueStreamOption(QDomElement root)
{
	if(!mTissueStreamDataAdapter)
	{
		bool defaultValue = false;
		mTissueStreamDataAdapter = BoolDataAdapterXml::initialize("ge_get_tissue_stream", "Tissue stream", "Request the tissue stream, the doppler data is removed.", defaultValue);
//		connect(mBufferSizeDataAdapter.get(), SIGNAL(valueWasSet()), this, SLOT(updateBufferSize()));
//		this->updateBufferSize();
	}

	return mTissueStreamDataAdapter;
}

BoolDataAdapterXmlPtr	GEStreamerService::getBandwidthStreamOption(QDomElement root)
{
	if(!mBandwidthStreamDataAdapter)
	{
		bool defaultValue = true;
		mBandwidthStreamDataAdapter = BoolDataAdapterXml::initialize("ge_get_bandwidth_stream", "Bandwidth stream", "Request the bandwidth stream data.", defaultValue);
//		connect(mBufferSizeDataAdapter.get(), SIGNAL(valueWasSet()), this, SLOT(updateBufferSize()));
//		this->updateBufferSize();
	}

	return mBandwidthStreamDataAdapter;
}

BoolDataAdapterXmlPtr	GEStreamerService::getFrequencyStreamOption(QDomElement root)
{
	if(!mFrequencyStreamDataAdapter)
	{
		bool defaultValue = false;
		mFrequencyStreamDataAdapter = BoolDataAdapterXml::initialize("ge_get_frequency_stream", "Frequency stream", "Request the frequency stream data.", defaultValue);
//		connect(mBufferSizeDataAdapter.get(), SIGNAL(valueWasSet()), this, SLOT(updateBufferSize()));
//		this->updateBufferSize();
	}

	return mFrequencyStreamDataAdapter;
}

BoolDataAdapterXmlPtr	GEStreamerService::getVelocityStreamOption(QDomElement root)
{
	if(!mVelocityStreamDataAdapter)
	{
		bool defaultValue = false;
		mVelocityStreamDataAdapter = BoolDataAdapterXml::initialize("ge_get_velocity_stream", "Velocity stream", "Request the velocity stream data.", defaultValue);
//		connect(mBufferSizeDataAdapter.get(), SIGNAL(valueWasSet()), this, SLOT(updateBufferSize()));
//		this->updateBufferSize();
	}

	return mVelocityStreamDataAdapter;
}

} /* namespace cx */
