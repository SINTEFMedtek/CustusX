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
	m_rMpr(new Transform3D(Transform3D::Identity())),
	mConfigured(false),
	mInitialized(false),
	mIsTracking(false)
{
	DummyToolPtr tool1(new DummyTool(this));

	mDominantTool = tool1;
	mReferenceTool = tool1;

	mDummyTools->insert(std::pair<QString, DummyToolPtr>(tool1->getUid(),
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
void DummyToolManager::uninitialize()
{
  mInitialized = false;
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
		retval->insert(std::pair<QString, ToolPtr>
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
		retval->insert(std::pair<QString, ToolPtr> (((*it).first), (ToolPtr)((*it).second)));
		it++;
	}
	return retval;
}
ToolPtr DummyToolManager::getTool(const QString& uid)
{
	DummyToolMapConstIter it = mDummyTools->find(uid);
	return (*it).second;
}

ToolPtr DummyToolManager::getDominantTool()
{
	return mDominantTool;
}
void DummyToolManager::setDominantTool(const QString& uid)
{
	DummyToolMapConstIter it = mDummyTools->find(uid);
	mDominantTool = (*it).second;
	emit dominantToolChanged(uid);
}

std::map<QString, QString> DummyToolManager::getToolUidsAndNames() const
{
	std::map<QString, QString> uidsAndNames;

	DummyToolMapConstIter it = mDummyTools->begin();
	while(it != mDummyTools->end())
	{
		uidsAndNames.insert(std::pair<QString, QString>(
                           ((*it).second)->getUid(),((*it).second)->getName()));
		it++;
	}
	return uidsAndNames;
}
std::vector<QString> DummyToolManager::getToolNames() const
{
	std::vector<QString> names;
	DummyToolMapConstIter it = mDummyTools->begin();
	while(it != mDummyTools->end())
	{
		names.push_back(((*it).second)->getName());
		it++;
	}
	return names;
}
std::vector<QString> DummyToolManager::getToolUids() const
{
	std::vector<QString> uids;
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
void DummyToolManager::saveTransformsAndTimestamps(QString filePathAndName)
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
