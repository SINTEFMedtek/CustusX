#include "sscDummyToolManager.h"

namespace ssc
{

//ToolManager* ToolManager::mInstance = NULL;

ToolManager* DummyToolManager::getInstance()
{
	if(ToolManager::mInstance == NULL)
	{
		ToolManager::mInstance = new DummyToolManager();
	}
	return ToolManager::mInstance;
}
DummyToolManager::DummyToolManager() :
	mDummyTools(new DummyToolMap),
	m_rMpr(new Transform3D()),
	mConfigured(false),
	mInitialized(false),
	mIsTracking(false)
{
	DummyToolPtr tool1(new DummyTool());

	mDominantTool = tool1;
	mReferenceTool = tool1;

	mDummyTools->insert(std::pair<std::string, DummyToolPtr>(tool1->getUid(),
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

	DummyToolMapConstIter it = mDummyTools->begin();
	while(it != mDummyTools->end())
	{
		((*it).second)->startTracking();
		it++;
	}

}
void DummyToolManager::stopTracking()
{
	mIsTracking = false;
	emit trackingStopped();

	DummyToolMapConstIter it = mDummyTools->begin();
	while(it != mDummyTools->end())
	{
		((*it).second)->stopTracking();
		it++;
	}
}

ToolManager::ToolMapPtr DummyToolManager::getConfiguredTools()
{
	//Is there a better way to do this?
	ToolMapPtr retval(new ToolMap);
	DummyToolMapIter it = mDummyTools->begin();
	while(it != mDummyTools->end())
	{
		retval->insert(std::pair<std::string, ToolPtr>
                     (((*it).first), (ToolPtr)((*it).second)));
		it++;
	}
	return retval;
}

ToolManager::ToolMapPtr DummyToolManager::getInitializedTools()
{
  //TODO?
  return ToolMapPtr(new ToolMap);
}

ToolManager::ToolMapPtr DummyToolManager::getTools()
{
	//Is there a better way to do this?
	ToolMapPtr retval(new ToolMap);
	DummyToolMapIter it = mDummyTools->begin();	
	while(it != mDummyTools->end())
	{
		retval->insert(std::pair<std::string, ToolPtr> (((*it).first), (ToolPtr)((*it).second)));
		it++;
	}
	return retval;
}
ToolPtr DummyToolManager::getTool(const std::string& uid)
{
	DummyToolMapConstIter it = mDummyTools->find(uid);
	return (*it).second;
}

ToolPtr DummyToolManager::getDominantTool()
{
	return mDominantTool;
}
void DummyToolManager::setDominantTool(const std::string& uid)
{
	DummyToolMapConstIter it = mDummyTools->find(uid);
	mDominantTool = (*it).second;
	emit dominantToolChanged(uid);
}

std::map<std::string, std::string> DummyToolManager::getToolUidsAndNames() const
{
	std::map<std::string, std::string> uidsAndNames;

	DummyToolMapConstIter it = mDummyTools->begin();
	while(it != mDummyTools->end())
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
	DummyToolMapConstIter it = mDummyTools->begin();
	while(it != mDummyTools->end())
	{
		names.push_back(((*it).second)->getName());
		it++;
	}
	return names;
}
std::vector<std::string> DummyToolManager::getToolUids() const
{
	std::vector<std::string> uids;
	DummyToolMapConstIter it = mDummyTools->begin();
	while(it != mDummyTools->end())
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

void DummyToolManager::set_rMpr(const Transform3DPtr& val)
{
	m_rMpr = val;
}

ToolPtr DummyToolManager::getReferenceTool() const
{
	return mReferenceTool;
}
void DummyToolManager::saveTransformsAndTimestamps(std::string filePathAndName)
{
	DummyToolMapConstIter it = mDummyTools->begin();
	while(it != mDummyTools->end())
	{
		((*it).second)->saveTransformsAndTimestamps();
		it++;
	}
}

DummyToolManager* DummyToolManager::getDowncastInstance()
{
	return dynamic_cast<DummyToolManager*>( getInstance() );
}

void DummyToolManager::addTool(DummyToolPtr tool)
{
	mDummyTools->insert(std::make_pair(tool->getUid(), tool));
}



} //namespace ssc
