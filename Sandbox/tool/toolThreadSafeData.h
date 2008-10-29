#ifndef TOOLTHREADSAFEDATA_H_
#define TOOLTHREADSAFEDATA_H_

#include <vector>
#include <set>
#include <boost/shared_ptr.hpp>
#include <QtCore>
#include "QtUtilities.h"
#include "Utilities.h"

// --------------------------------------------------------
namespace tool
{
// --------------------------------------------------------

/**Data object that stores all data in the position tracking manager.
 * The data is guarded by a mutex, thus the object is threadsafe.
 */
class ThreadSafeData : public QObject
{
	Q_OBJECT
public:
	ThreadSafeData() : mMutex(QMutex::Recursive) {}
	Transform3D position(const std::string& uid) const
	{
		QMutexLocker locker(&mMutex);
		return mToolData.find(uid)->second.position;		
	}
	bool visible(const std::string& uid) const
	{
		QMutexLocker locker(&mMutex);
		return mToolData.find(uid)->second.visible;				
	}
	int id(const std::string& uid) const
	{
		QMutexLocker locker(&mMutex);
		return mToolData.find(uid)->second.id;						
	}
	int systemId(const std::string& uid) const
	{
		QMutexLocker locker(&mMutex);
		return mToolData.find(uid)->second.systemId;								
	}
	std::set<int> systems() const
	{
		QMutexLocker locker(&mMutex);
		return mSystems;
	}
	void setToolData(int sysID, int toolID, int toolStatus, const Transform3D& toolPos)
	{
		QMutexLocker locker(&mMutex);
		std::string uid = createUid(sysID,toolID);
		std::map<std::string,ToolData> ::iterator iter = mToolData.find(uid);
		if (iter==mToolData.end())
		{
			mToolData[uid] = ToolData();
			iter = mToolData.find(uid);
			iter->second.id = toolID;
			iter->second.systemId = sysID;		
			emit toolAdded(qstring_cast(uid));
		}
		
		if (iter->second.visible != (toolStatus==0))
		{
			iter->second.visible = (toolStatus==0);			
			Logger::log("tool log", string_cast(uid) + " vis: " + string_cast(iter->second.visible));		
			emit toolVisibilityChanged(qstring_cast(uid));			
		}
		if (iter->second.visible && (!similar(iter->second.position, toolPos))) // only show pos if visible
		{
			iter->second.position = toolPos;
			emit toolPosChanged(qstring_cast(uid));
			Logger::log("tool log", string_cast(uid) + " pos: " + string_cast(iter->second.position));		
		}
	}
	bool addSystem(int id) 
	{
		QMutexLocker locker(&mMutex);
		if (mSystems.count(id))
			return false;
		mSystems.insert(id);
		Logger::log("tool log", "added system "+string_cast(id));
		return true;
	}
	
signals:
	void toolPosChanged(const QString& uid);
	void toolVisibilityChanged(const QString& uid);
	void toolAdded(const QString& uid);
	//void statusMessage(const QString& msg);
	
private:
	struct ToolData
	{
		Transform3D position;
		bool visible;
		int id;
		int systemId;		
	};

	std::string createUid(int sysID, int toolID) const
	{
		return "tool_" + string_cast(sysID) + ":" + string_cast(toolID);
	}
	std::set<int> mSystems;	
	std::map<std::string,ToolData> mToolData;
	mutable QMutex mMutex; ///< guard for all internal data.
};

typedef boost::shared_ptr<ThreadSafeData> ThreadSafeDataPtr;

// --------------------------------------------------------
}
// --------------------------------------------------------

#endif /*TOOLTHREADSAFEDATA_H_*/
