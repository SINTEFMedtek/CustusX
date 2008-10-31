#ifndef SSCTOOLMANAGER_H_
#define SSCTOOLMANAGER_H_

#include <set>
#include <map>
#include <vector>
#include <QObject>
#include "sscTransform3D.h"
#include "sscTool.h"

namespace ssc
{

class ToolManager : public QObject
{
	Q_OBJECT
public:
	typedef std::set<ToolPtr> ToolSet;

	static void setInstance(ToolManager* instance)
	{
		mInstance = instance;
	};
	//static ToolManager* getInstance() = 0;

	virtual bool isConfigured() const = 0; ///< system is ready to use but not connected to hardware
	virtual bool isInitialized() const = 0; ///< system is connected to hw and ready
	virtual bool isTracking() const = 0; ///< system is tracking

	virtual void configure() = 0;
	virtual void initialize() = 0;
	virtual void startTracking() = 0;
	virtual void stopTracking() = 0;

	virtual ToolSet getConfiguredTools() = 0; ///< get all configured tools
	virtual ToolSet getTools() = 0; ///< get connected tools
	virtual ToolPtr getTool(const std::string& uid) = 0;

	virtual ToolPtr getDominantTool() = 0;
	virtual void setDominantTool(const std::string& uid) = 0;

	virtual std::map<std::string, std::string> getToolUidsAndNames() const = 0;
	virtual std::vector<std::string> getToolNames() const = 0;
	virtual std::vector<std::string> getToolUids() const = 0;

	virtual Transform3DPtr get_rMpr() const = 0; ///< transform from patient ref to ref space
	virtual ToolPtr getReferenceTool() const = 0;
	virtual void saveTransformsAndTimestamps(std::string filePathAndName = "") = 0;

signals:
	void configured();
	void initialized();
	void trackingStarted();
	void trackingStopped();

protected:
	ToolManager(){};
	~ToolManager(){};

	static ToolManager* mInstance;
};

//M_r_tool = toolManager->get_M_ref_pat() * currentTool->get_M_pat_tool();
//rMt = toolManager->get_rMp() * currentTool->get_pMt();

} //namespace ssc
#endif /* SSCTOOLMANAGER_H_ */
