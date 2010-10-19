/*
 * cxApplicationState.cpp
 *
 *  Created on: Aug 21, 2010
 *      Author: christiana
 */

#include "cxApplicationState.h"



#include <iostream>
#include <QState>
#include <QStateMachine>
#include <QString>
#include <QAction>
#include "sscTypeConversions.h"
//#include "sscDataManager.h"
#include "cxRequestEnterStateTransition.h"
//#include "cxStateMachineManager.h"
//#include "cxPatientData.h"
//#include "cxViewManager.h"
#include "sscMessageManager.h"
#include "sscDataManager.h"
#include "cxStateMachineManager.h"
#include "cxPatientData.h"

namespace cx
{

ApplicationState::ApplicationState(QState* parent, QString uid, QString name) :
  QState(parent),
  mUid(uid), mName(name), mAction(NULL), mActive(false)
{
};

ApplicationState::~ApplicationState()
{
}

void ApplicationState::onEntry(QEvent * event )
{
  mActive = true;
  ssc::messageManager()->sendInfo("Application change to [" + mName + "]");
  if(mAction)
    mAction->setChecked(true);

  ssc::dataManager()->setMedicalDomain(this->getMedicalDomain());
  if (stateManager()->getWorkflow())
    stateManager()->getWorkflow()->setActiveState("PatientDataUid");
  stateManager()->getPatientData()->clearPatient();
};

void ApplicationState::onExit(QEvent * event )
{
  mActive = false;
  //std::cout << "Exiting application "<< mName << std::endl;
};

QString ApplicationState::getUid() const
{
  return mUid;
};

QString ApplicationState::getName() const
{
  return mName;
};

std::vector<ApplicationState*> ApplicationState::getChildStates()
{
  QObjectList childrenList = this->children();
  std::vector<ApplicationState*> retval;
  for (int i=0; i<childrenList.size(); ++i)
  {
    ApplicationState* state = dynamic_cast<ApplicationState*>(childrenList[i]);
    if (state)
      retval.push_back(state);
  }
  return retval;
}

QAction* ApplicationState::createAction(QActionGroup* group)
{
  if(mAction)
    return mAction;

  mAction = new QAction(this->getName(), group);
  mAction->setCheckable(true);
  mAction->setChecked(mActive);
  mAction->setData(QVariant(this->getUid()));
  //this->canEnterSlot();

  connect(mAction, SIGNAL(triggered()), this, SLOT(setActionSlot()));

  return mAction;
};

void ApplicationState::setActionSlot()
{
  this->machine()->postEvent(new RequestEnterStateEvent(this->getUid()));
};


} // namespace cx


