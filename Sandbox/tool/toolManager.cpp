#include "toolManager.h"
#include "toolPositionTrackingThread.h"
#include "toolToolImpl.h"
#include "toolThreadSafeData.h"

// --------------------------------------------------------
namespace tool
{
// --------------------------------------------------------

// --------------------------------------------------------
// static member
ToolManager* ToolManager::mInstance = NULL;
// --------------------------------------------------------

ToolManager* ToolManager::instance()
{
	if (!mInstance)
	{
		mInstance = new ToolManager();
	}
	return mInstance;
}

void ToolManager::init()
{
	mThread->sendInit();
}

ToolManager::ToolManager() : mToolMapMutex(QMutex::Recursive)
{
	mData.reset(new ThreadSafeData());
	// connect signals to slots:
	connect(mData.get(), SIGNAL(toolPosChanged(const QString&)), this, SLOT(toolPosChangedSlot(const QString&)));
	connect(mData.get(), SIGNAL(toolVisibilityChanged(const QString&)), this, SLOT(toolVisibilityChangedSlot(const QString&)));
	connect(mData.get(), SIGNAL(toolAdded(const QString&)), this, SLOT(toolAddedSlot(const QString&)));
	// forward signals:
	connect(mData.get(), SIGNAL(toolPosChanged(const QString&)), this, SIGNAL(toolPosChanged(const QString&)));
	connect(mData.get(), SIGNAL(toolVisibilityChanged(const QString&)), this, SIGNAL(toolVisibilityChanged(const QString&)));
	connect(mData.get(), SIGNAL(toolAdded(const QString&)), this, SIGNAL(toolAdded(const QString&)));

	mThread = new PositionTrackingThread(mData);
	
	//mDebugWindow.reset(new ToolManagerStatusWindow());
	//connect(mData.get(), SIGNAL(statusMessage(const QString&)), this, SLOT(statusMessageSlot(const QString&)));	
	//mDebugWindow->show();
	
	mThread->start();
}

ToolManager::~ToolManager()
{
	mThread->quit();
}

void ToolManager::startTracking()
{
	mThread->sendStartTracking();	
}

//void ToolManager::addObserver(boost::function<void()> changedObserver)
//{
//	
//}

void ToolManager::shutdown()
{
	// TODO implement
}

void ToolManager::toolPosChangedSlot(const QString& uid)
{
	get(uid)->invokeTransformEvent();
}

void ToolManager::toolVisibilityChangedSlot(const QString& uid)
{
	get(uid)->invokeVisibleEvent();
}

void ToolManager::toolAddedSlot(const QString& uid)
{
	get(uid); // ensure the tool exists in list.
}

ToolImplPtr ToolManager::get(const QString& uid)
{
	return get(uid.toStdString());
}

ToolImplPtr ToolManager::get(const std::string& uid)
{
	QMutexLocker locker(&mToolMapMutex);

	if (!mTools.count(uid))
	{
		mTools[uid].reset(new ToolImpl(uid, mData));		
	}
	return mTools[uid];
}

ssc::ToolPtr ToolManager::tool(const std::string& uid)
{
	return get(uid);
}

std::vector<ssc::ToolPtr> ToolManager::tools()
{
	QMutexLocker locker(&mToolMapMutex);

	std::vector<ssc::ToolPtr> retval;
	for (ToolMap::iterator iter=mTools.begin(); iter!=mTools.end(); ++iter)
	{
		retval.push_back(iter->second);
	}
	return retval;
}
//std::vector<int> ToolManager::systems() const
//{
//	
//}

std::vector<std::string> ToolManager::toolUids() const
{
	QMutexLocker locker(&mToolMapMutex);

	std::vector<std::string> retval;
	for (ToolMap::const_iterator iter=mTools.begin(); iter!=mTools.end(); ++iter)
	{
		retval.push_back(iter->first);
	}
	return retval;	
}

bool ToolManager::initialized() const
{
	// TODO implement
	return true;
}

void ToolManager::stopTracking()
{
	mThread->sendStopTracking();		
}

ssc::ToolPtr ToolManager::activeTool()
{
	return ssc::ToolPtr();
	// TODO
}

// --------------------------------------------------------
// --------------------------------------------------------
// --------------------------------------------------------

// --------------------------------------------------------
// --------------------------------------------------------
// --------------------------------------------------------

// --------------------------------------------------------
} // namespace tool
// --------------------------------------------------------
