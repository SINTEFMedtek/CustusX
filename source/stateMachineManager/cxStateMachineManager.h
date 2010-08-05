#ifndef CXSTATEMACHINEMANAGER_H_
#define CXSTATEMACHINEMANAGER_H_

#include <QDomNode>

#include "cxForwardDeclarations.h"
#include "cxWorkflowStateMachine.h"

namespace cx
{
/**
 * \class cxStateMachineManager.h
 *
 * \brief
 *
 * \date 4. aug. 2010
 * \author: Janne Beate Bakeng, SINTEF
 */
class StateMachineManager
{
public:
  StateMachineManager();
  ~StateMachineManager();

  WorkflowStateMachinePtr getWorkflow();

  //Interface for saving/loading
  void addXml(QDomNode& dataNode); ///< adds xml information about the statemachinemanager and its variabels
  void parseXml(QDomNode& dataNode);///< Use a XML node to load data. \param dataNode A XML data representation of the statemachinemanager.

private:
  WorkflowStateMachinePtr mWorkflowStateMachine;
};

typedef boost::shared_ptr<StateMachineManager> StateMachineManagerPtr;
}
#endif /* CXSTATEMACHINEMANAGER_H_ */
