#include "toolPositionTrackingThread.h"

namespace tool
{


PositionTrackingThread::PositionTrackingThread(const ThreadSafeDataPtr& data, QObject* parent) : 
	mMutex(QMutex::Recursive),
	QThread(parent)
{
	this->setObjectName("PositionTrackingThread");
	mData = data;
	mDBus.reset(new snw_utils::DBusWrapper("com.sonowand.cpp.toolmanager"));
	
	mPollTimer = new QTimer(this);
	connect(mPollTimer, SIGNAL(timeout()), this, SLOT(pollTimeout()));
	mPollTimer->start(0);	
}

PositionTrackingThread::~PositionTrackingThread() 
{
	mDBus.reset();
}

void PositionTrackingThread::sendInit()
{
	QMutexLocker locker(&mMutex);
	mDBus->signal_start(POS_TRACK_PING);
	mDBus->signal_send();
}

void PositionTrackingThread::newSystem(int sysId)
{
	if (!mData->addSystem(sysId))
		return;

	QMutexLocker locker(&mMutex);
	
	mDBus->signal_start(POS_TRACK_INIT);
	mDBus->signal_add_int(sysId);
	mDBus->signal_send();
	
	mDBus->signal_start(POS_TRACK_TOOL_REQUEST);
	mDBus->signal_add_int(sysId);
	mDBus->signal_send();	
}

void PositionTrackingThread::sendStartTracking()
{
	std::set<int> systems = mData->systems();
	
	QMutexLocker locker(&mMutex);
	for (std::set<int>::iterator iter=systems.begin(); iter!=systems.end(); ++iter)
	{
		mDBus->signal_start(POS_TRACK_START_TRACKING);
		mDBus->signal_add_int(*iter);
		mDBus->signal_send();
	}
}

void PositionTrackingThread::sendStopTracking()
{
	// TODO: not sure if this is in the Navigator spec.
	std::set<int> systems = mData->systems();
	
	QMutexLocker locker(&mMutex);
	for (std::set<int>::iterator iter=systems.begin(); iter!=systems.end(); ++iter)
	{
		mDBus->signal_start(POS_TRACK_PAUSE);
		mDBus->signal_add_int(*iter);
		mDBus->signal_send();
	}
}

void PositionTrackingThread::pollTimeout()
{
	poll();
}
//
//void PositionTrackingThread::run()
//{
//	while ( !mStopped)
//	{
//		poll();
//	}
//}

/**Read a Transform3D from the current dbus signal.
 * Input format is one int whhich must be 16
 * plus one 16-size double arrat
 */
Transform3D PositionTrackingThread::readTransformFromDBus()
{
	int matrixSize = mDBus->signal_get_int();
	//snw_assert(matrixSize==16, "error in matrixsize");

	boost::array<double, 16> matrix;
	mDBus->signal_get_double_array(matrix.begin(), matrix.size());

	return Transform3D(matrix.begin());
}

void PositionTrackingThread::poll()
{
	QMutexLocker locker(&mMutex);
	
	SNWDBUS_ENUM sig = mDBus->get_signal();

	switch (sig)
	{
	case 0:
	{
		locker.unlock();
		usleep(20000);
		//mNavigation->idle();
	}
		break;
	case POS_TRACK_PONG:
	{
		int sysId = mDBus->signal_get_int();
		locker.unlock();
		newSystem(sysId);		
	}
	case POS_TRACK_TOOL_POS: // 721
	{
		/**From Position Tracking DDS:
		 * I32 PosSystID
		 * I32 ToolID
		 * U32 Timestamp
		 * I32 ToolStatus
		 * ARRAY[DBL] TxMatrix [if status==0]
		 */
		int32_t sysID = mDBus->signal_get_int();
		int32_t toolID = mDBus->signal_get_int();
		mDBus->signal_get_uint(); // timestamp
		int32_t toolStatus = mDBus->signal_get_int();
		Transform3D toolPos;
		if (toolStatus == 0) // only sent if status ok
		{
			Transform3D toolPos = readTransformFromDBus();
		}
		locker.unlock();
		mData->setToolData(sysID, toolID, toolStatus, toolPos);
	}
		break;
	default:
		; // do nothing
		break;
	} // switch (sig)
}



} // namespace tool
