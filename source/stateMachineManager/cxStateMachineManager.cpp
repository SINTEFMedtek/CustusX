#include "cxStateMachineManager.h"

namespace cx
{

StateMachineManager::StateMachineManager()
{
  mWorkflowStateMachine.reset(new WorkflowStateMachine());
  mWorkflowStateMachine->start();
}

StateMachineManager::~StateMachineManager()
{}

WorkflowStateMachinePtr StateMachineManager::getWorkflow()
{
  return mWorkflowStateMachine;
}

void StateMachineManager::addXml(QDomNode& dataNode)
{}

void StateMachineManager::parseXml(QDomNode& dataNode)
{}

} //namespace cx
