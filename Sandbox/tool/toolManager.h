#ifndef TOOLMANAGER_H_
#define TOOLMANAGER_H_

#include <vector>
#include <set>
#include <map>
#include <boost/shared_ptr.hpp>
#include <QtCore>

#include "sscTool.h"
#include "Utilities.h"

// --------------------------------------------------------
namespace tool
{
// --------------------------------------------------------
typedef boost::shared_ptr<class ToolImpl> ToolImplPtr;
typedef boost::shared_ptr<class ThreadSafeData> ThreadSafeDataPtr;

/**Manager for all tools in a system.
 * Singleton.
 * Runs an internal thread that synchronizes with any 
 * external hardware/processes. It is threadsafe, i.e. users 
 * can call methods without worrying about the underlying 
 * implementation.
 */
class ToolManager : public QObject
{
	Q_OBJECT
public:	
	void init();
	void shutdown();
	ssc::ToolPtr tool(const std::string& uid);
	ssc::ToolPtr activeTool();
	std::vector<ssc::ToolPtr> tools();
	//std::vector<int> systems() const;
	std::vector<std::string> toolUids() const;
	bool initialized() const;
	void startTracking();
	void stopTracking();
	
	//void addObserver(boost::function<void()> changedObserver); // will be called in internal thread.
	
	static ToolManager* instance();
	
private slots:	
	void toolPosChangedSlot(const QString& uid);
	void toolVisibilityChangedSlot(const QString& uid);
	void toolAddedSlot(const QString& uid);
signals:
	void toolPosChanged(const QString& uid);
	void toolVisibilityChanged(const QString& uid);
	void toolAdded(const QString& uid);
private:
	ToolManager();
	~ToolManager();
	
	ToolImplPtr get(const QString& uid);
	ToolImplPtr get(const std::string& uid);
	
	class PositionTrackingThread* mThread;
	typedef std::map<std::string, ToolImplPtr> ToolMap;
	ToolMap mTools;
	ThreadSafeDataPtr mData;
	//boost::shared_ptr<class ToolManagerStatusWindow> mDebugWindow;
	static ToolManager* mInstance;
	
	mutable QMutex mToolMapMutex; ///< protects the map mTools
};


// --------------------------------------------------------
} // namespace tool
// --------------------------------------------------------
#endif /*TOOLMANAGER_H_*/
