#include "cxErrorObserver.h"
#include <memory>

namespace cx {

//---------------------------------------------------------
StaticMutexVtkLocker::StaticMutexVtkLocker()
{
/*	if (!mMutex)
		mMutex.reset(new QMutex(QMutex::Recursive));

	mMutex->lock();*/
}
StaticMutexVtkLocker::~StaticMutexVtkLocker()
{
//	mMutex->unlock();
}
std::shared_ptr<QMutex> StaticMutexVtkLocker::mMutex;

}
