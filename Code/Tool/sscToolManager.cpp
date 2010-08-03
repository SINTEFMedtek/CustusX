#include "sscToolManager.h"

namespace ssc
{

ToolManager* toolManager() { return ToolManager::getInstance(); }

ToolManager* ToolManager::mInstance = NULL;

/** not sure if this is needed? we have getInstance in subclasses...*/
void ToolManager::setInstance(ToolManager* instance)
{
 mInstance = instance;
};

ToolManager* ToolManager::getInstance()
{
  return mInstance;
}

void ToolManager::shutdown()
{
 delete mInstance;
 mInstance = NULL;
}

}
