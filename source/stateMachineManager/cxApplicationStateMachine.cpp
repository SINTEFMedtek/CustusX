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
  ApplicationState* Laparoscopy = this->newState(new LaparoscopyApplicationState(mParentState));
  ApplicationState* lung = this->newState(new LungApplicationState(mParentState));
  ApplicationState* endovascular = this->newState(new EndovascularApplicationState(mParentState));

  Q_UNUSED(neurology);
  Q_UNUSED(Laparoscopy);
  Q_UNUSED(lung);
  Q_UNUSED(endovascular);

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

QStringList ApplicationStateMachine::getAllApplicationNames()
{
  QStringList retval;
  ApplicationStateMap::iterator it = mStates.begin();
  for(; it != mStates.end(); ++it)
    retval << it->second->getName();
  return retval;
}


}//namespace cx
