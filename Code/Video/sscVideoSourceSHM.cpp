#include "sscVideoSourceSHM.h"

#include <boost/array.hpp>
#include <vector>

#include <vtkImageData.h>
#include <vtkImageImport.h>
#include <vtkPNGReader.h>


#include <qtimer.h>

namespace ssc
{

VideoSourceSHM::VideoSourceSHM(int width, int height, int depth)
	: mImageWidth(width), mImageHeight(height), mImageColorDepth(depth), mImageImport(vtkImageImportPtr::New())
{
	// this->setEmptyImage();
	mImageImport->SetDataScalarTypeToUnsignedChar();
	mImageImport->SetNumberOfScalarComponents(3);

	mConnected = false;
	mStreaming = false;

	mPollTimer = new QTimer(this);
	mPollTimer->setInterval(40);	// Polling interval (currently set @ 25 fps)
	mTimeStamp = 0;


	mImageData = mImageImport->GetOutput();
}

VideoSourceSHM::~VideoSourceSHM()
{
	// Disconnect
	disconnectServer();
}

QString VideoSourceSHM::getUid()
{
	return mSource.key();
}

QString VideoSourceSHM::getName()
{
	return "SHMLink";
}

vtkImageDataPtr VideoSourceSHM::getVtkImageData()
{
	return mImageImport->GetOutput();
}

double VideoSourceSHM::getTimestamp()
{
	return (double) mSource.timestamp().toMSecsSinceEpoch();
}

/// Releases the currently held shared memory client buffer
void VideoSourceSHM::release()
{
	mSource.release();
}

/// Returns a short info message
QString VideoSourceSHM::getInfoString() const
{
	return "NOT YET IMPLEMENTED";
}

/// Returns a short status message
QString VideoSourceSHM::getStatusString() const
{
	return QString("%1 and %2").arg(isConnected() ? "Connected" : "Not connected").arg(isStreaming() ? "streaming" : "not streaming");;
}

void VideoSourceSHM::start()
{
	if (mEnablePolling)
	{
		mPollTimer->start();
		// If all is well - tell the system we're streaming
		mStreaming = true;
	}

	emit streaming(mStreaming);
}

void VideoSourceSHM::stop()
{
	if (mEnablePolling)
	{
		mPollTimer->stop();
		// If all is well - tell the system we've stopped streaming
		mStreaming = false;
	}

	emit streaming(mStreaming);
}

bool VideoSourceSHM::validData() const
{
	// return (isConnected() && isStreaming()); // Currently only check available
	return true;
}

bool VideoSourceSHM::isConnected() const
{
	return mConnected;
}

bool VideoSourceSHM::isStreaming() const
{
	return (isConnected() && mStreaming);
}

/**
  * Grabs latest video source frame (if it exists) and signals a new frame
  * Must call release on this when buffer no longer is needed
  */
void VideoSourceSHM::update()
{
	unsigned char* buffer = (unsigned char*) mSource.isNew(); // Fetch new data from server - NULL if no new data present
	if (!buffer)
		return;

	mTimeStamp = mSource.timestamp().currentMSecsSinceEpoch();

	int numChannels = mImageColorDepth/sizeof(uchar);
	Q_UNUSED(numChannels);

	mImageImport->SetWholeExtent(0, mImageWidth - 1, 0, mImageHeight - 1, 0, 0);
	mImageImport->SetDataExtentToWholeExtent();

	mImageImport->SetImportVoidPointer(buffer, mImageWidth * mImageHeight);

	emit newFrame();
}

/**
  * Connects to a shared memory server end, described by a unique key string.
  * Connects signals and slots on success.
  */
void VideoSourceSHM::connectServer(const QString& key)
{
	mConnected = mSource.attach(key);

	if (mConnected)
	{
		connect(mPollTimer, SIGNAL(timeout()), this, SLOT(serverPollSlot()));
		mPollTimer->start();
	}
}

/**
  * Disconnects from current shared memory server end.
  * Disconnects signals and slots.
  */
void VideoSourceSHM::disconnectServer()
{
	stop();

	if (mConnected)
	{
		mPollTimer->stop();
		disconnect(mPollTimer, SIGNAL(timeout()), this, SLOT(serverPollSlot()));
		mSource.release();
	}

	mConnected = false;
}

/**
  * Sets current image import to hold empty content - size 1, 1
  */
void VideoSourceSHM::setEmptyImage()
{
	mImageImport->SetWholeExtent(0, 1, 0, 1, 0, 0);
	mImageImport->SetDataExtentToWholeExtent();

	boost::array<unsigned char, 100> zeroData;
	std::fill(zeroData.begin(), zeroData.end(), 0);

	mImageImport->SetImportVoidPointer(zeroData.begin());
	mImageImport->Modified();
}

/**
  * Slot: calls update on this
  */
void VideoSourceSHM::serverPollSlot()
{
	this->update();
}

} // end namespace
