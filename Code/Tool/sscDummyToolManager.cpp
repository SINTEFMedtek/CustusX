#include "sscDummyToolManager.h"

namespace ssc
{
ToolManager* ToolManager::mInstance = NULL;

ToolManager* DummyToolManager::getInstance()
{
	if(ToolManager::mInstance == NULL)
	{
		ToolManager::mInstance = new DummyToolManager();
	}
	return ToolManager::mInstance;
}
DummyToolManager::DummyToolManager() :
	mConfigured(false),
	mInitialized(false),
	mIsTracking(false),
	m_rMpr(new Transform3D())
{
	DummyToolPtr tool1(new DummyTool());

	mDominantTool = tool1;
	mReferenceTool = tool1;

	mDummyTools.insert(std::pair<std::string, DummyToolPtr>(tool1->getUid(),
                                                            tool1));
}
DummyToolManager::~ DummyToolManager()
{}
bool DummyToolManager::isConfigured() const
{
	return mConfigured;
}
bool DummyToolManager::isInitialized() const
{
	return mInitialized;
}
bool DummyToolManager::isTracking() const
{
	return mIsTracking;
}

void DummyToolManager::configure()
{
	mConfigured = true;
	emit configured();
}
void DummyToolManager::initialize()
{
	mInitialized = true;
	emit initialized();
}
void DummyToolManager::startTracking()
{
	mIsTracking = true;
	emit trackingStarted();

	DummyToolMapConstIter it = mDummyTools.begin();
	while(it != mDummyTools.end())
	{
		((*it).second)->startTracking();
		it++;
	}

}
void DummyToolManager::stopTracking()
{
	mIsTracking = false;
	emit trackingStopped();

	DummyToolMapConstIter it = mDummyTools.begin();
	while(it != mDummyTools.end())
	{
		((*it).second)->stopTracking();
		it++;
	}
}

ToolManager::ToolMap DummyToolManager::getConfiguredTools()
{
	//Is there a better way to do this?
	ToolMap retval;
	DummyToolMapIter it = mDummyTools.begin();
	while(it != mDummyTools.end())
	{
		retval.insert(std::pair<std::string, ToolPtr>
                     (((*it).first), (ToolPtr)((*it).second)));
		it++;
	}
	return retval;
}
ToolManager::ToolMap DummyToolManager::getTools()
{
	//Is there a better way to do this?
	ToolMap retval;
	DummyToolMapIter it = mDummyTools.begin();
	while(it != mDummyTools.end())
	{
		retval.insert(std::pair<std::string, ToolPtr>
                     (((*it).first), (ToolPtr)((*it).second)));
		it++;
	}
	return retval;
}
ToolPtr DummyToolManager::getTool(const std::string& uid)
{
	DummyToolMapConstIter it = mDummyTools.find(uid);
	return (*it).second;
}

ToolPtr DummyToolManager::getDominantTool()
{
	return mDominantTool;
}
void DummyToolManager::setDominantTool(const std::string& uid)
{
	DummyToolMapConstIter it = mDummyTools.find(uid);
	mDominantTool = (*it).second;
}

std::map<std::string, std::string> DummyToolManager::getToolUidsAndNames() const
{
	std::map<std::string, std::string> uidsAndNames;

	DummyToolMapConstIter it = mDummyTools.begin();
	while(it != mDummyTools.end())
	{
		uidsAndNames.insert(std::pair<std::string, std::string>(
                           ((*it).second)->getUid(),((*it).second)->getName()));
		it++;
	}
	return uidsAndNames;
}
std::vector<std::string> DummyToolManager::getToolNames() const
{
	std::vector<std::string> names;
	DummyToolMapConstIter it = mDummyTools.begin();
	while(it != mDummyTools.end())
	{
		names.push_back(((*it).second)->getName());
		it++;
	}
	return names;
}
std::vector<std::string> DummyToolManager::getToolUids() const
{
	std::vector<std::string> uids;
	DummyToolMapConstIter it = mDummyTools.begin();
	while(it != mDummyTools.end())
	{
		uids.push_back(((*it).second)->getUid());
		it++;
	}
	return uids;
}

Transform3DPtr DummyToolManager::get_rMpr() const
{
	return m_rMpr;
}
ToolPtr DummyToolManager::getReferenceTool() const
{
	return mReferenceTool;
}
void DummyToolManager::saveTransformsAndTimestamps(std::string filePathAndName)
{
	DummyToolMapConstIter it = mDummyTools.begin();
	while(it != mDummyTools.end())
	{
		((*it).second)->saveTransformsAndTimestamps();
		it++;
	}
}

} //namespace ssc
