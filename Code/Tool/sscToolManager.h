#ifndef SSCTOOLMANAGER_H_
#define SSCTOOLMANAGER_H_

#include <map>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <QObject>
#include "sscTransform3D.h"
#include "sscTool.h"

namespace ssc
{

class ToolManager : public QObject
{
	Q_OBJECT
public:
	typedef std::map<std::string, ToolPtr> ToolMap;
	typedef boost::shared_ptr<ToolMap> ToolMapPtr;

	/** not sure if this is needed? we have getInstance in subclasses...*/
	static void setInstance(ToolManager* instance)
	{	
		mInstance = instance;
	};
	static ToolManager* getInstance() { return mInstance; }	
	static void shutdown()
	{
		delete mInstance;
		mInstance = NULL;
	}
	
	
	virtual bool isConfigured() const = 0; ///< system is ready to use but not connected to hardware
	virtual bool isInitialized() const = 0; ///< system is connected to hw and ready
	virtual bool isTracking() const = 0; ///< system is tracking

	virtual void configure() = 0; ///< get the system ready without connecting to the tracking hardware
	virtual void initialize() = 0; ///< connect the system to the tracking hardware
	virtual void startTracking() = 0; ///< start tracking
	virtual void stopTracking() = 0; ///< stop tracking

	virtual ToolMapPtr getConfiguredTools() = 0; ///< get all configured tools
	virtual ToolMapPtr getTools() = 0; ///< get connected tools
	virtual ToolPtr getTool(const std::string& uid) = 0; ///< get a tool

	virtual ToolPtr getDominantTool() = 0; ///< get the tool that has higest priority when tracking
	virtual void setDominantTool(const std::string& uid) = 0; ///< set a tool to be the dominant tool

	virtual std::map<std::string, std::string> getToolUidsAndNames() const = 0; ///< get all tools uids and names
	virtual std::vector<std::string> getToolNames() const = 0; ///< get the name of all tools
	virtual std::vector<std::string> getToolUids() const = 0; ///< get the uid of all the tools

	virtual Transform3DPtr get_rMpr() const = 0; ///< transform from patient ref to ref space
	virtual ToolPtr getReferenceTool() const = 0; ///< tool used as patient reference
	/** write to file all recorded transforms and timestamps */
	virtual void saveTransformsAndTimestamps(std::string filePathAndName = "") = 0;

signals:
	void configured(); ///< signal emitted when the system is configured
	void initialized(); ///< signal emitted when the system is initialized
	void trackingStarted(); ///< signal emitted when the system starts tracking
	void trackingStopped(); ///< signal emitted when the system stops tracking
	void dominantToolChanged(const std::string& uId); ///<signal for change of dominant tool
	
protected:
	ToolManager(){}; ///< Empty on purpose
	~ToolManager(){}; ///< Empty on purpose

	static ToolManager* mInstance; ///< The only instance of this class that can exist.
};

//M_r_tool = toolManager->get_M_ref_pat() * currentTool->get_M_pat_tool();
//rMt = toolManager->get_rMp() * currentTool->get_pMt();

} //namespace ssc
#endif /* SSCTOOLMANAGER_H_ */
