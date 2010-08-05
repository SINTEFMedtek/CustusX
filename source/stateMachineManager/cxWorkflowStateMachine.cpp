#include "cxWorkflowStateMachine.h"
#include <QAbstractTransition>
#include <QMenu>
#include "cxWorkflowState.h"
#include "cxRequestEnterStateTransition.h"

namespace cx
{

WorkflowStateMachine::WorkflowStateMachine()
{
  QState* topState = new QState();
  this->addState(topState);
  this->setInitialState(topState);

  PatientDataWorkflowState* pd1 = new PatientDataWorkflowState(topState);
  NavigationWorkflowState* n1 = new NavigationWorkflowState(topState);

  this->newState(pd1);
  this->newState(n1);

  RequestEnterStateTransition* tpd1 = new RequestEnterStateTransition(pd1->getUid());
  tpd1->setTargetState(pd1);
  topState->addTransition(tpd1);

  RequestEnterStateTransition* tn1 = new RequestEnterStateTransition(n1->getUid());
  tn1->setTargetState(n1);
  topState->addTransition(tn1);

  topState->setInitialState(pd1);
}

void WorkflowStateMachine::newState(WorkflowState* state)
{
  //this->addState(state);
  mStates[state->getUid()] = state;
}

WorkflowStateMachine::~WorkflowStateMachine()
{}

QActionGroup* WorkflowStateMachine::getActionGroup()
{
  QActionGroup* retval = new QActionGroup(NULL);
  retval->setExclusive(true);

  for (WorkflowStateMap::iterator iter=mStates.begin(); iter!=mStates.end(); ++iter)
  {
    iter->second->createAction(retval);
  }

  return retval;
}

void WorkflowStateMachine::fillMenu(QMenu* menu)
{
  menu->addActions(this->getActionGroup()->actions());
}

}//namespace cx
