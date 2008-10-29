#ifndef TOOLPOSITIONTRACKINGTHREAD_H_
#define TOOLPOSITIONTRACKINGTHREAD_H_

#include <vector>
#include <set>
#include <map>
#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>
#include <QtCore>
#include "toolToolImpl.h"

#include "Utilities.h"
#include "DBusWrapper.h"
#include "toolThreadSafeData.h"

namespace tool
{


class PositionTrackingThread : public QThread
{
	Q_OBJECT
public:
	PositionTrackingThread(const ThreadSafeDataPtr& data, QObject* parent=NULL); 
	~PositionTrackingThread();
	//void run();
	//void stop();
	
	void sendInit(); ///< initialize position tracking. thread-safe
	void sendStartTracking(); ///< thread-safe
	void sendStopTracking(); ///< thread-safe
	
private:
	QTimer* mPollTimer;
	void poll();
	void newSystem(int sysId);
	Transform3D readTransformFromDBus();

	ThreadSafeDataPtr mData;
	snw_utils::DBusWrapperPtr mDBus;	
	mutable QMutex mMutex; ///< guard for dbus read/write 
	//bool mStopped;
public slots:
	void pollTimeout();	
};
// --------------------------------------------------------

} // namespace tool

#endif /*TOOLPOSITIONTRACKINGTHREAD_H_*/
