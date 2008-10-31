#include "sscDummyToolManager.h"

namespace ssc
{
ToolManager* ToolManager::mInstance = NULL;
//void DummyToolManager::setInstance(ToolManager* instance)
//{}
//ToolManager* DummyToolManager::getInstance()
//{}
DummyToolManager::DummyToolManager() :
	mConfigured(false),
	mInitialized(false),
	mIsTracking(false),
	m_rMpr(new Transform3D())
{
	//ToolPtr referenceTool(new DummyTool());
	DummyToolPtr tool1(new DummyTool());
	//ToolPtr tool2(new DummyTool());

	mDominantTool = tool1;
	mReferenceTool = tool1;

	//mTools.insert(referenceTool);
	mDummyTools.insert(tool1);
	//mTools.insert(tool2);
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
}
void DummyToolManager::initialize()
{
	mInitialized = true;
}
void DummyToolManager::startTracking()
{
	mIsTracking = true;

	DummyToolSetIter it = mDummyTools.begin();
	while(it != mDummyTools.end())
	{
		(*it)->startTracking();
		it++;
	}

}
void DummyToolManager::stopTracking()
{
	mIsTracking = false;

	DummyToolSetIter it = mDummyTools.begin();
	while(it != mDummyTools.end())
	{
		(*it)->stopTracking();
		it++;
	}
}

ToolManager::ToolSet DummyToolManager::getConfiguredTools()
{
	//return mDummyTools;
}
ToolManager::ToolSet DummyToolManager::getTools()
{
	//return mDummyTools;
}
ToolPtr DummyToolManager::getTool(const std::string& uid)
{
	DummyToolSetIter it = mDummyTools.begin();
	while(it != mDummyTools.end())
	{
		if((*it)->getUid() == uid)
		{
			return (*it);
		}
		it++;
	}
}

ToolPtr DummyToolManager::getDominantTool()
{
	return mDominantTool;
}
void DummyToolManager::setDominantTool(const std::string& uid)
{
	DummyToolSetIter it = mDummyTools.begin();
	while(it != mDummyTools.end())
	{
		if((*it)->getUid() == uid)
		{
			mDominantTool = (*it);
		}
		it++;
	}
}

std::map<std::string, std::string> DummyToolManager::getToolUidsAndNames() const
{
	std::map<std::string, std::string> uidsAndNames;
	DummyToolSetIter it = mDummyTools.begin();
	while(it != mDummyTools.end())
	{
		uidsAndNames.insert(std::pair<std::string, std::string>((*it)->getUid(), (*it)->getName()));
		it++;
	}
	return uidsAndNames;
}
std::vector<std::string> DummyToolManager::getToolNames() const
{
	std::vector<std::string> names;
	DummyToolSetIter it = mDummyTools.begin();
	while(it != mDummyTools.end())
	{
		names.push_back((*it)->getName());
		it++;
	}
	return names;
}
std::vector<std::string> DummyToolManager::getToolUids() const
{
	std::vector<std::string> uids;
	DummyToolSetIter it = mDummyTools.begin();
	while(it != mDummyTools.end())
	{
		uids.push_back((*it)->getUid());
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
	DummyToolSetIter it = mDummyTools.begin();
	while(it != mDummyTools.end())
	{
		(*it)->saveTransformsAndTimestamps();
		it++;
	}
}

} //namespace ssc
