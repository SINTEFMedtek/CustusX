/*
 * cxApplicationStateMachine.cpp
 *
 *  Created on: Aug 17, 2010
 *      Author: christiana
 */

#include "cxApplicationStateMachine.h"
#include <QAbstractTransition>
#include <QMenu>
#include <QToolBar>
#include <QSettings>
#include <cxDataLocations.h>
#include "cxApplicationState.h"
#include "cxRequestEnterStateTransition.h"

namespace cx
{

ApplicationStateMachine::ApplicationStateMachine()
{
  mActionGroup = new QActionGroup(NULL);

  mParentState = new ParentApplicationState(this);

  ApplicationState* laboratory = this->newState(new LaboratoryApplicationState(mParentState));
  ApplicationState* neurology = this->newState(new NeurologyApplicationState(mParentState));
  ApplicationState* laparascopy = this->newState(new LaparascopyApplicationState(mParentState));
  ApplicationState* lung = this->newState(new LungApplicationState(mParentState));

  Q_UNUSED(neurology);
  Q_UNUSED(laparascopy);
  Q_UNUSED(lung);

  //set initial state on all levels
  this->setInitialState(mParentState);

  QSettingsPtr settings = DataLocations::getSettings();
  QString initState = settings->value("globalApplicationName").toString();
  if (mStates.count(initState))
    mParentState->setInitialState(mStates[initState]);
  else
    mParentState->setInitialState(laboratory);
}

ApplicationState* ApplicationStateMachine::newState(ApplicationState* state)
{
  RequestEnterStateTransition* transToState = new RequestEnterStateTransition(state->getUid());
  transToState->setTargetState(state);
  mParentState->addTransition(transToState);

  //must call internal slot first! This updates internal state before informing the world.
  connect(state , SIGNAL(entered()), this, SLOT(activeStateChangedSlot()));
  connect(state , SIGNAL(entered()), this, SIGNAL(activeStateChanged()));

  mStates[state->getUid()] = state;
  return state;
}

ApplicationStateMachine::~ApplicationStateMachine()
{}

void ApplicationStateMachine::activeStateChangedSlot()
{
  QSettingsPtr settings = DataLocations::getSettings();
  settings->setValue("globalApplicationName", this->getActiveUidState());
}

QActionGroup* ApplicationStateMachine::getActionGroup()
{
  QString active = this->getActiveUidState();
  mActionGroup->setExclusive(true);
  //TODO rebuild action list when we need dynamic lists. Must rethink memory management then.
  for (ApplicationStateMap::iterator iter=mStates.begin(); iter!=mStates.end(); ++iter)
  {
    iter->second->createAction(mActionGroup);
  }

  return mActionGroup;
}

//void ApplicationStateMachine::fillMenu(QMenu* menu)
//{
//  this->fillMenu(menu, mParentState);
//}
//
//void ApplicationStateMachine::fillMenu(QMenu* menu, WorkflowState* current)
//{
//  std::vector<WorkflowState*> childStates = current->getChildStates();
//
//  if (childStates.empty())
//  {
//    menu->addAction(current->createAction(mActionGroup));
//  }
//  else // current is a node. create submenu and fill in recursively
//  {
//    QMenu* submenu = menu;
//    if (current!=mParentState) // ignore creation of submenu for parent state
//      submenu = menu->addMenu(current->getName());
//    for (unsigned i=0; i<childStates.size(); ++i)
//      this->fillMenu(submenu, childStates[i]);
//  }
//}


//void ApplicationStateMachine::fillToolBar(QToolBar* toolbar)
//{
//  this->fillToolbar(toolbar, mParentState);
//}
//
//void ApplicationStateMachine::fillToolbar(QToolBar* toolbar, WorkflowState* current)
//{
//  std::vector<WorkflowState*> childStates = current->getChildStates();
//
//  if (childStates.empty())
//  {
//    toolbar->addAction(current->createAction(mActionGroup));
//  }
//  else // current is a node. fill in recursively
//  {
//    for (unsigned i=0; i<childStates.size(); ++i)
//      this->fillToolbar(toolbar, childStates[i]);
//  }
//}


QString ApplicationStateMachine::getActiveUidState()
{
  QSet<QAbstractState *> states = this->configuration();
  for(QSet<QAbstractState *>::iterator iter=states.begin(); iter!=states.end(); ++iter)
  {
    ApplicationState* wfs = dynamic_cast<ApplicationState*>(*iter);
    if(!wfs || !wfs->getChildStates().empty())
      continue;

    return wfs->getUid();
  }
  return QString();
}

QString ApplicationStateMachine::getActiveStateName()
{
  QString uid = this->getActiveUidState();
  if (!mStates.count(uid))
    return "";
  return mStates[uid]->getName();
}


}//namespace cx
