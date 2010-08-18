#include "cxWorkflowStateMachine.h"
#include <QAbstractTransition>
#include <QMenu>
#include <QToolBar>
#include "cxWorkflowState.h"
#include "cxRequestEnterStateTransition.h"

namespace cx
{

WorkflowStateMachine::WorkflowStateMachine()
{
  mActionGroup = new QActionGroup(NULL);

  mParentState = new ParentWorkflowState(this);

  WorkflowState* patientData = this->newState(new PatientDataWorkflowState(mParentState));
  WorkflowState* registration = this->newState(new RegistrationWorkflowState(mParentState));
  WorkflowState* imageRegistration= this->newState(new ImageRegistrationWorkflowState(registration));
  WorkflowState* patientRegistration = this->newState(new PatientRegistrationWorkflowState(registration));
  WorkflowState* preOpPlanning = this->newState(new PreOpPlanningWorkflowState(mParentState));
  WorkflowState* navigation = this->newState(new NavigationWorkflowState(mParentState));
  WorkflowState* intraOpImaging = this->newState(new IntraOpImagingWorkflowState(mParentState));
  WorkflowState* postOpControll = this->newState(new PostOpControllWorkflowState(mParentState));

  Q_UNUSED(registration);
  Q_UNUSED(patientRegistration);
  Q_UNUSED(preOpPlanning);
  Q_UNUSED(navigation);
  Q_UNUSED(intraOpImaging);
  Q_UNUSED(postOpControll);

  //set initial state on all levels
  this->setInitialState(mParentState);
  mParentState->setInitialState(patientData);
  registration->setInitialState(imageRegistration);
}

WorkflowState* WorkflowStateMachine::newState(WorkflowState* state)
{
  RequestEnterStateTransition* transToState = new RequestEnterStateTransition(state->getUid());
  transToState->setTargetState(state);
  mParentState->addTransition(transToState);

  connect(state , SIGNAL(entered()), this, SIGNAL(activeStateChanged()));

  mStates[state->getUid()] = state;
  return state;
}

WorkflowStateMachine::~WorkflowStateMachine()
{}

QActionGroup* WorkflowStateMachine::getActionGroup()
{
  mActionGroup->setExclusive(true);
  //TODO rebuild action list when we need dynamic lists. Must rethink memory management then.
  for (WorkflowStateMap::iterator iter=mStates.begin(); iter!=mStates.end(); ++iter)
  {
    iter->second->createAction(mActionGroup);
  }

  return mActionGroup;
}

void WorkflowStateMachine::fillMenu(QMenu* menu)
{
  this->fillMenu(menu, mParentState);
}

void WorkflowStateMachine::fillMenu(QMenu* menu, WorkflowState* current)
{
  std::vector<WorkflowState*> childStates = current->getChildStates();

  if (childStates.empty())
  {
    menu->addAction(current->createAction(mActionGroup));
  }
  else // current is a node. create submenu and fill in recursively
  {
    QMenu* submenu = menu;
    if (current!=mParentState) // ignore creation of submenu for parent state
      submenu = menu->addMenu(current->getName());
    for (unsigned i=0; i<childStates.size(); ++i)
      this->fillMenu(submenu, childStates[i]);
  }
}


void WorkflowStateMachine::fillToolBar(QToolBar* toolbar)
{
  this->fillToolbar(toolbar, mParentState);
}

void WorkflowStateMachine::fillToolbar(QToolBar* toolbar, WorkflowState* current)
{
  std::vector<WorkflowState*> childStates = current->getChildStates();

  if (childStates.empty())
  {
    toolbar->addAction(current->createAction(mActionGroup));
  }
  else // current is a node. fill in recursively
  {
    for (unsigned i=0; i<childStates.size(); ++i)
      this->fillToolbar(toolbar, childStates[i]);
  }
}


QString WorkflowStateMachine::getActiveUidState()
{
  QSet<QAbstractState *> states = this->configuration();
  for(QSet<QAbstractState *>::iterator iter=states.begin(); iter!=states.end(); ++iter)
  {
    WorkflowState* wfs = dynamic_cast<WorkflowState*>(*iter);
    if(!wfs || !wfs->getChildStates().empty())
      continue;

    return wfs->getUid();
  }
  return QString();
}

}//namespace cx
