	#ifndef SSCDUMMYTOOLMANAGER_H_
#define SSCDUMMYTOOLMANAGER_H_

#include "sscToolManager.h"
#include "sscDummyTool.h"
/**
 * sscDummyToolManager.h
 *
 * \brief
 *
 * \date Oct 29, 2008
 * \author: jbake
 */

namespace ssc
{
class DummyToolManager : public ToolManager
{
	Q_OBJECT

public:
	typedef std::map<std::string, DummyToolPtr> DummyToolMap;
	typedef boost::shared_ptr<DummyToolMap> DummyToolMapPtr;

	static ToolManager* getInstance();

	virtual bool isConfigured() const;
	virtual bool isInitialized() const;
	virtual bool isTracking() const;

	virtual void configure();
	virtual void initialize();
	virtual void startTracking();
	virtual void stopTracking();

	virtual ToolMapPtr getConfiguredTools();
	virtual ToolMapPtr getTools();
	virtual ToolPtr getTool(const std::string& uid);

	virtual ToolPtr getDominantTool();
	virtual void setDominantTool(const std::string& uid);

	virtual std::map<std::string, std::string> getToolUidsAndNames() const;
	virtual std::vector<std::string> getToolNames() const;
	virtual std::vector<std::string> getToolUids() const;

	virtual Transform3DPtr get_rMpr() const;
	virtual void set_rMpr(const Transform3DPtr& val);
	virtual ToolPtr getReferenceTool() const;
	virtual void saveTransformsAndTimestamps(std::string filePathAndName = "");

private:
	typedef DummyToolMap::iterator DummyToolMapIter;
	typedef DummyToolMap::const_iterator DummyToolMapConstIter;

	DummyToolManager();
	~DummyToolManager();

	DummyToolMapPtr mDummyTools;
	DummyToolPtr mDominantTool;
	DummyToolPtr mReferenceTool;

	Transform3DPtr m_rMpr;

	bool mConfigured;
	bool mInitialized;
	bool mIsTracking;
};

}//namespace ssc

#endif /* SSCDUMMYTOOLMANAGER_H_ */
