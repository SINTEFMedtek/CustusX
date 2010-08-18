#ifndef CXWORKFLOWSTATE_H_
#define CXWORKFLOWSTATE_H_

#include <iostream>
#include <QState>
#include <QStateMachine>
#include <QString>
#include <QAction>
#include "sscTypeConversions.h"
#include "sscDataManager.h"
#include "cxRequestEnterStateTransition.h"
#include "cxStateMachineManager.h"
#include "cxPatientData.h"
#include "cxViewManager.h"
#include "sscMessageManager.h"

namespace cx
{
/**
 * \class cxWorkflowState.h
 *
 * \brief
 *
 * \date 4. aug. 2010
 * \author: jbake
 */

class WorkflowState : public QState
{
  Q_OBJECT

public:
  WorkflowState(QState* parent, QString uid, QString name) :
    QState(parent),
    mUid(uid), mName(name), mAction(NULL)
  {
  };

  virtual ~WorkflowState(){};

  virtual void onEntry(QEvent * event )
  {
    ssc::messageManager()->sendInfo("Workflow change to [" + string_cast(mName) + "]");
    if(mAction)
      mAction->setChecked(true);
  };
  virtual void onExit(QEvent * event )
  {
    //std::cout << "Exiting "<< mName << std::endl;
  };

  virtual QString getUid() const {return mUid;};
  virtual QString getName() const {return mName;};

  std::vector<WorkflowState*> getChildStates()
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


  QAction* createAction(QActionGroup* group)
  {
    if(mAction)
      return mAction;

    mAction = new QAction(this->getName(), group);
    mAction->setCheckable(true);
    mAction->setData(QVariant(this->getUid()));
    this->canEnterSlot();

    connect(mAction, SIGNAL(triggered()), this, SLOT(setActionSlot()));

    return mAction;
  };

protected slots:
  void canEnterSlot()
  {
    if (mAction) mAction->setEnabled(this->canEnter());
  };

  void setActionSlot()
  {
    this->machine()->postEvent(new RequestEnterStateEvent(this->getUid()));
  };

protected:
  virtual bool canEnter() const = 0;

  QString mUid;
  QString mName;
  QAction* mAction;
};

class ParentWorkflowState : public WorkflowState
{
  Q_OBJECT
public:
  ParentWorkflowState(QState* parent) :
  WorkflowState(parent, "ParentUid", "Parent") { }
  virtual ~ParentWorkflowState(){};
  virtual void onEntry(QEvent * event ) { }
  virtual void onExit(QEvent * event ) { }
  virtual bool canEnter() const {return true;}
};

class PatientDataWorkflowState : public WorkflowState
{
  Q_OBJECT

public:
  PatientDataWorkflowState(QState* parent) :
  WorkflowState(parent, "PatientDataUid", "Patient Data")
  {  };

  virtual ~PatientDataWorkflowState(){};

  virtual bool canEnter() const {return true;};
};

class NavigationWorkflowState : public WorkflowState
{
  Q_OBJECT

public:
  NavigationWorkflowState(QState* parent) :
  WorkflowState(parent, "NavigationUid", "Navigation")
  {
    connect(stateManager()->getPatientData().get(), SIGNAL(patientChanged()), this, SLOT(canEnterSlot()));
  };

  virtual ~NavigationWorkflowState(){};

  virtual bool canEnter() const {return stateManager()->getPatientData()->isPatientValid();};
};

class RegistrationWorkflowState : public WorkflowState
{
  Q_OBJECT

public:
  RegistrationWorkflowState(QState* parent) :
  WorkflowState(parent, "RegistrationUid", "Registration")
  {
    connect(ssc::dataManager(), SIGNAL(dataLoaded()), this, SLOT(canEnterSlot()));
  };

  virtual ~RegistrationWorkflowState(){};

  virtual bool canEnter() const
  {
    return !ssc::dataManager()->getImages().empty();
  };
};

class ImageRegistrationWorkflowState : public WorkflowState
{
  Q_OBJECT

public:
  ImageRegistrationWorkflowState(QState* parent) :
  WorkflowState(parent, "ImageRegistrationUid", "Image Registration")
  {
    connect(ssc::dataManager(), SIGNAL(dataLoaded()), this, SLOT(canEnterSlot()));
  };

  virtual ~ImageRegistrationWorkflowState(){};

  virtual bool canEnter() const
  {
    return !ssc::dataManager()->getImages().empty();
  };

  virtual void onEntry(QEvent * event )
  {
    WorkflowState::onEntry(event);
    viewManager()->setRegistrationMode(ssc::rsIMAGE_REGISTRATED);
  };

  virtual void onExit(QEvent * event )
  {
    WorkflowState::onExit(event);
    viewManager()->setRegistrationMode(ssc::rsNOT_REGISTRATED);
  };

  /*
  //TODO: MOVE TO SHIFT CORRECTION... and Finish ShiftCorrection
  QString imagesPath = mPatientData->getActivePatientFolder()+"/Images";
  mShiftCorrectionWidget->init(imagesPath);
  //Don't show ShiftCorrection in release
  //mShiftCorrectionIndex = mContextDockWidget->addTab(mShiftCorrectionWidget,
  //    QString("Shift correction"));
*/
};

class PatientRegistrationWorkflowState : public WorkflowState
{
  Q_OBJECT

public:
  PatientRegistrationWorkflowState(QState* parent) :
  WorkflowState(parent, "PatientRegistrationUid", "Patient Registration")
  {
    connect(ssc::dataManager(), SIGNAL(dataLoaded()), this, SLOT(canEnterSlot()));
  };

  virtual ~PatientRegistrationWorkflowState(){};

  virtual bool canEnter() const
  {
    //TODO check that data is image registrated
    return !ssc::dataManager()->getImages().empty();
  };
  virtual void onEntry(QEvent * event )
  {
    WorkflowState::onEntry(event);
    viewManager()->setRegistrationMode(ssc::rsPATIENT_REGISTRATED);
  };

  virtual void onExit(QEvent * event )
  {
    WorkflowState::onExit(event);
    viewManager()->setRegistrationMode(ssc::rsNOT_REGISTRATED);
  };
};

class PreOpPlanningWorkflowState : public WorkflowState
{
  Q_OBJECT

public:
  PreOpPlanningWorkflowState(QState* parent) :
  WorkflowState(parent, "PreOpPlanningUid", "Pre Op Planning")
  {  };

  virtual ~PreOpPlanningWorkflowState(){};

  virtual bool canEnter() const
  {
    return !ssc::dataManager()->getImages().empty();
  }
};

class IntraOpImagingWorkflowState : public WorkflowState
{
  Q_OBJECT

public:
  IntraOpImagingWorkflowState(QState* parent) :
  WorkflowState(parent, "IntraOpImagingUid", "Intra Op Imaging")
  {  };

  virtual ~IntraOpImagingWorkflowState(){};

  virtual bool canEnter() const
  {
    return stateManager()->getPatientData()->isPatientValid();
  }
};

class PostOpControllWorkflowState : public WorkflowState
{
  Q_OBJECT

public:
  PostOpControllWorkflowState(QState* parent) :
  WorkflowState(parent, "PostOpControllUid", "Post Op Controll")
  {  };

  virtual ~PostOpControllWorkflowState(){};

  virtual bool canEnter() const
  {
    return !ssc::dataManager()->getImages().empty();
  }
};

}
#endif /* CXWORKFLOWSTATE_H_ */
