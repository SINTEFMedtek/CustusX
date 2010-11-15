#include "vmUltrasoundPlane.h"

#include <math.h>
#include <vtkImageData.h>
#include <vtkImageImport.h>
#include <vtkDataSetMapper.h>
#include <vtkTimerLog.h>

#include "Utilities.h"

namespace vm
{

UltrasoundSource::UltrasoundSource() :
	mImageWidth( 0),
	mImageHeight( 0),
	mImageImport(vtkImageImportPtr::New())
{
	mUSSession = -1;
	mUSTextBuf = NULL;

	mImageImport->SetNumberOfScalarComponents(4);
	mImageImport->SetDataScalarTypeToUnsignedChar();

	mImageData = mImageImport->GetOutput();
}

UltrasoundSource::~UltrasoundSource()
{
	disconnect();
}

bool UltrasoundSource::connected() const
{
	return (mUSSession != -1);
}

void UltrasoundSource::reconnect()
{
	if (!connected())
		return;
	int temp = m_shmtKey;
	disconnect();
	connect(temp);
}

/**connect to shared memory
 */
void UltrasoundSource::connect(int shmtKey)
{
	if (mUSSession != -1)
	{
		disconnect();
	}

	m_shmtKey = shmtKey;
	int err = SHM_Connect(shmtKey, &mUSSession);

	if (err != kNoError)
	{
		mUSSession = -1;
		SW_LOG( "Failed connecting to shared buffer with key %d, status: %d", shmtKey, err );
		return;
	}
	SW_LOG( "Connected to shared buffer with key %d", shmtKey );

	update();
}

/**disconnect from shared memory
 */
void UltrasoundSource::disconnect()
{
	if (mUSSession != -1)
	{
		SHM_Done(mUSSession);
		mUSSession = -1;
	}

	initializeBuffer(0,0);
}

void UltrasoundSource::update()
{
	if (!connected())
	{
		return;
	}

	snw_us::UltrasoundImage *us_image= NULL;

	int32_t dummy_index;
	us_image = ( snw_us::UltrasoundImage* ) SHM_GetReadBuffer(mUSSession, &dummy_index);
	if (!us_image)
	{
		SW_LOG("US Image couldn't be loaded.");
		return;
	}

	initializeBuffer(us_image->width, us_image->height);

	// This is where the image is decoded
	if (snw_us::DecodeUltrasoundImage(mUSTextBuf, mImageWidth, mImageHeight, *us_image) != kNoError)
	{
		SW_LOG( "US Image couldn't be decoded." );
		return;
	}

//	std::stringstream ss;
//	ss << "connected(): " << connected() << std::endl;
//	ss << "mImageWidth: " << mImageWidth << std::endl;
//	ss << "mImageHeight: " << mImageHeight << std::endl;
//	ss << "us_image->width: " << us_image->width << std::endl;
//	ss << "us_image->height: " << us_image->height << std::endl;
//	Logger::state("vm.us.state", ss.str());

// debug code: speckle the image with pink in order to see better
//	for (int i=0; i<mImageWidth*mImageHeight; ++i)
//	{
//		if (i%4==0)
//			//printf("mUSTextBuf[i] = %d\n" , mUSTextBuf[i] );
//			//mUSTextBuf[i] = 0xFFFF00FF ;
//	}


//	Logger::log("vm.log", "us update "+ string_cast(mImageWidth) + ", " + string_cast(mImageHeight) );
	mImageImport->Modified();
}

/**Change size of buffers if changed (or new)
 * Connect to mImageImport.
 */
void UltrasoundSource::initializeBuffer(int newWidth, int newHeight)
{
	padBox(&newWidth, &newHeight); // remove to use input bb

	// already ok: ignore
	if (mImageWidth==newWidth && mImageHeight==newHeight)
	{
		return;
	}

	mImageWidth = newWidth;
	mImageHeight = newHeight;

	mImageImport->SetWholeExtent(0, mImageWidth - 1, 0, mImageHeight - 1, 0, 0);
	mImageImport->SetDataExtentToWholeExtent();
	int size = mImageWidth * mImageHeight;
	delete[] mUSTextBuf;
	mUSTextBuf = NULL;
	if (size>0)
	{
		mUSTextBuf = new uint32_t[size];
		memset(mUSTextBuf, 0, sizeof(int32_t) * size);
		mImageImport->SetImportVoidPointer(mUSTextBuf);
		Logger::log("vm.log", "connected us buffer to vtkImageImport, size=("+string_cast(mImageWidth) + "," + string_cast(mImageHeight)+")");
	}
	else
	{
		mImageImport->SetImportVoidPointer(NULL);
		Logger::log("vm.log", "disconnected us buffer from vtkImageImport");
	}
}

vtkImageDataPtr UltrasoundSource::getVtkImageData()
{
	return mImageData;
}

/** increase input bounding box to a power of 2.
 */
void UltrasoundSource::padBox(int* x, int* y) const
{
	if (*x==0 || *y==0)
		return;

	int x_org = *x;
	int y_org = *y;

	*x = 1;
	*y = 1;
	while (*x < x_org)
		(*x) *= 2;
	while (*y < y_org)
		(*y) *= 2;
}


//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------

UltrasoundData::UltrasoundData()
{
	mModified = false;
	mCurrentThermIndexVal = -1.0;
	mCurrentMechIndexVal = -1.0;
	mSource.reset(new UltrasoundSource());


	mTimeLimit = 1000;  // evaluation time(milliseconds) for us image
	mValidImage = true;

	mQtimer = new QTimer(this);
	connect( mQtimer, SIGNAL(timeout()),this, SLOT(clearDisplay()) );
}

void UltrasoundData::statusModified()
{
	emit statusChanged();
}

/**Update screen if modified.
 * The reason for this system is that we want to
 * receive the updated message from US Scanner first,
 * then empty the message queue (i.e. dbus), then render.
 * A timer is added to check if image is older then 1.0 second
 */
void UltrasoundData::refresh()
{
	if (!mModified)
	{
		return;
	}
	display();
	mModified = false;
}
void UltrasoundData::startTimer()
{
	mQtimer->start(static_cast<int>(mTimeLimit));
}

void UltrasoundData::clearDisplay()
{
	if ( mStatus == snw_us::Status_ConnectedFrozen )
	{
		return;
	}
	mValidImage = false;
	setStatus(snw_us::Status_ErrorExpiredImage , -1.0,-1.0 );
}
void UltrasoundData::display()
{
	int w = mSource->width();
	int h = mSource->height();

	mSource->update();
	if (w!=mSource->width() || h!=mSource->height())
	{
		emit statusChanged();
	}
}

void UltrasoundData::setStatus(snw_us::ModuleStatus status, double mi, double ti)
{
	if( fabs( mi ) > 0.01 )
	{
		mCurrentMechIndexVal = mi;
	}
	if( fabs( ti ) > 0.01 )
	{
		mCurrentThermIndexVal = ti;
	}

	emit statusChanged();
	mStatus = status;
}

double UltrasoundData::mechanicalIndex() const
{
	return  mCurrentMechIndexVal;
}
double UltrasoundData::thermalIndex() const
{
	return mCurrentThermIndexVal;
}

std::string UltrasoundData::statusString() const
{
	if ( connected() && mValidImage )
	{
		return "";//"US OK - (debug msg)";
	}
	switch( mStatus )
	{
		case snw_us::Status_ConnectedProbeless:
			return "No probe in current connector";
		case snw_us::Status_EstablishingConnection:
			return "Establishing connection";
		case snw_us::Status_ConnectedLive :
			return "Connected";
		case snw_us::Status_ConnectedFrozen:
			return "Frozen";
		case snw_us::Status_ErrorNoConnection:
			return "No connection";
		case snw_us::Status_ErrorConnectionRefused :
			return "Trying to connect";
		case snw_us::Status_ErrorExpiredImage :
			return "Refreshing image";
		default : return "No connection.";
	}
}

/**set the state to modified, meaning that next call to refresh
 * will update screen anew.
 */
void UltrasoundData::setModified()
{
	mQtimer->stop();

	if (!mValidImage)
	{
		mValidImage = true;
		emit statusChanged();
	}

	mModified = true;
}

void UltrasoundData::connectSource(int shmtKey)
{
	mSource->connect(shmtKey);
	emit statusChanged();
	setModified();
}
void UltrasoundData::disconnectSource()
{
	mSource->disconnect();
	emit statusChanged();
	setModified();
}

void UltrasoundData::reconnectSource()
{
	mSource->reconnect();
	emit statusChanged();
	setModified();
}

bool UltrasoundData::valid()
{
	if ( mValidImage && connected())
	{
		return true;
	}
	return false;
}
bool UltrasoundData::connected() const
{
	return mSource->connected();
}

vtkImageDataPtr UltrasoundData::getVtkImageData()
{
	return mSource->getVtkImageData();
}

int UltrasoundData::width() const
{
	return mSource->width();
}

int UltrasoundData::height() const
{
	return mSource->height();
}



}
