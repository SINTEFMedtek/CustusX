#include "cxWorkflowState.h"
#include "RTSource/cxRTSourceManager.h"
#include "sscDataManager.h"
#include "cxStateMachineManager.h"
#include "cxPatientData.h"
#include "cxSettings.h"
#include "sscToolManager.h"

namespace cx
{


void WorkflowState::onEntry(QEvent * event )
{
  ssc::messageManager()->sendInfo("Workflow change to [" + mName + "]");
  if(mAction)
    mAction->setChecked(true);
};

void WorkflowState::onExit(QEvent * event )
{
  emit aboutToExit();
};

std::vector<WorkflowState*> WorkflowState::getChildStates()
{
  QObjectList childrenList = this->children();
  std::vector<WorkflowState*> retval;
  for (int i=0; i<childrenList.size(); ++i)
  {
    WorkflowState* state = dynamic_cast<WorkflowState*>(childrenList[i]);
    if (state)
      retval.push_back(state);
  }
  return retval;
}

QAction* WorkflowState::createAction(QActionGroup* group)
{
  if(mAction)
    return mAction;

  mAction = new QAction(this->getName(), group);
  mAction->setIcon(this->getIcon());
  mAction->setStatusTip(this->getName());
  mAction->setCheckable(true);
  mAction->setData(QVariant(this->getUid()));
  this->canEnterSlot();

  connect(mAction, SIGNAL(triggered()), this, SLOT(setActionSlot()));

  return mAction;
};

void WorkflowState::canEnterSlot()
{
  if (mAction) mAction->setEnabled(this->canEnter());
};

void WorkflowState::setActionSlot()
{
  this->machine()->postEvent(new RequestEnterStateEvent(this->getUid()));
};

void WorkflowState::autoStartHardware()
{
  if (settings()->value("Automation/autoStartTracking").toBool())
    ssc::toolManager()->startTracking();
  if (settings()->value("Automation/autoStartStreaming").toBool())
    stateManager()->getRTSourceManager()->launchAndConnectServer();
}


// --------------------------------------------------------
// --------------------------------------------------------

// --------------------------------------------------------
// --------------------------------------------------------

NavigationWorkflowState::NavigationWorkflowState(QState* parent) :
    WorkflowState(parent, "NavigationUid", "Navigation")
{
  connect(stateManager()->getPatientData().get(), SIGNAL(patientChanged()), this, SLOT(canEnterSlot()));
};

void NavigationWorkflowState::onEntry(QEvent * event)
{
  this->autoStartHardware();
}

bool NavigationWorkflowState::canEnter() const
{
  return stateManager()->getPatientData()->isPatientValid();
}

// --------------------------------------------------------
// --------------------------------------------------------

// --------------------------------------------------------
// --------------------------------------------------------

RegistrationWorkflowState::RegistrationWorkflowState(QState* parent) :
  WorkflowState(parent, "RegistrationUid", "Registration")
{
  connect(ssc::dataManager(), SIGNAL(dataLoaded()), this, SLOT(canEnterSlot()));
};

bool RegistrationWorkflowState::canEnter() const
{
  return !ssc::dataManager()->getImages().empty();
};

// --------------------------------------------------------
// --------------------------------------------------------

// --------------------------------------------------------
// --------------------------------------------------------


PreOpPlanningWorkflowState::PreOpPlanningWorkflowState(QState* parent) :
  WorkflowState(parent, "PreOpPlanningUid", "Preoperative Planning")
{
  connect(ssc::dataManager(), SIGNAL(dataLoaded()), this, SLOT(canEnterSlot()));
}

bool PreOpPlanningWorkflowState::canEnter() const
{
  return !ssc::dataManager()->getImages().empty();
}

// --------------------------------------------------------
// --------------------------------------------------------

// --------------------------------------------------------
// --------------------------------------------------------


IntraOpImagingWorkflowState::IntraOpImagingWorkflowState(QState* parent) :
  WorkflowState(parent, "IntraOpImagingUid", "Intraoperative Imaging")
{
  connect(stateManager()->getPatientData().get(), SIGNAL(patientChanged()), this, SLOT(canEnterSlot()));
}

void IntraOpImagingWorkflowState::onEntry(QEvent * event)
{
  this->autoStartHardware();
}

bool IntraOpImagingWorkflowState::canEnter() const
{
  return stateManager()->getPatientData()->isPatientValid();
}


// --------------------------------------------------------
// --------------------------------------------------------

// --------------------------------------------------------
// --------------------------------------------------------

PostOpControllWorkflowState::PostOpControllWorkflowState(QState* parent) :
  WorkflowState(parent, "PostOpControllUid", "Postoperative Control")
{
  connect(ssc::dataManager(), SIGNAL(dataLoaded()), this, SLOT(canEnterSlot()));
}

bool PostOpControllWorkflowState::canEnter() const
{
  return !ssc::dataManager()->getImages().empty();
}


}

