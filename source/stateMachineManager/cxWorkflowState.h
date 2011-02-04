#ifndef CXWORKFLOWSTATE_H_
#define CXWORKFLOWSTATE_H_

#include <iostream>
#include <QState>
#include <QStateMachine>
#include <QString>
#include <QAction>
#include "sscTypeConversions.h"
#include "cxRequestEnterStateTransition.h"
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

  virtual void onEntry(QEvent * event );
  virtual void onExit(QEvent * event )
  {
    //std::cout << "Exiting "<< mName << std::endl;
  };

  virtual QString getUid() const {return mUid;};
  virtual QString getName() const {return mName;};

  std::vector<WorkflowState*> getChildStates();
  QAction* createAction(QActionGroup* group);

protected slots:
  void canEnterSlot();
  void setActionSlot();

protected:
  virtual bool canEnter() const = 0;
  virtual QIcon getIcon() const = 0;
  void autoStartHardware();

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
  virtual QIcon getIcon() const { return QIcon(""); }
};

class PatientDataWorkflowState : public WorkflowState
{
  Q_OBJECT

public:
  PatientDataWorkflowState(QState* parent) :
  WorkflowState(parent, "PatientDataUid", "Patient Data")
  {  };

  virtual ~PatientDataWorkflowState(){};
  virtual QIcon getIcon() const { return QIcon(":/icons/workflow_state_patient_data.png"); }

  virtual bool canEnter() const {return true;};
};

class NavigationWorkflowState : public WorkflowState
{
  Q_OBJECT

public:
  NavigationWorkflowState(QState* parent);
  virtual ~NavigationWorkflowState(){};
  virtual QIcon getIcon() const { return QIcon(":/icons/workflow_state_navigation.png"); }
  virtual void onEntry(QEvent* event);
  virtual bool canEnter() const;
};

class RegistrationWorkflowState : public WorkflowState
{
  Q_OBJECT

public:
  RegistrationWorkflowState(QState* parent);

  virtual ~RegistrationWorkflowState(){};
  virtual QIcon getIcon() const { return QIcon(":/icons/workflow_state_registration.png"); }

  virtual bool canEnter() const;
};

/*class ImageRegistrationWorkflowState : public WorkflowState
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


  //TODO: MOVE TO SHIFT CORRECTION... and Finish ShiftCorrection
  QString imagesPath = mPatientData->getActivePatientFolder()+"/Images";
  mShiftCorrectionWidget->init(imagesPath);
  //Don't show ShiftCorrection in release
  //mShiftCorrectionIndex = mContextDockWidget->addTab(mShiftCorrectionWidget,
  //    QString("Shift correction"));

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
};*/

class PreOpPlanningWorkflowState : public WorkflowState
{
  Q_OBJECT

public:
  PreOpPlanningWorkflowState(QState* parent);

  virtual ~PreOpPlanningWorkflowState(){};
  virtual QIcon getIcon() const { return QIcon(":/icons/workflow_state_planning.png"); }

  virtual bool canEnter() const;
};

class IntraOpImagingWorkflowState : public WorkflowState
{
  Q_OBJECT

public:
  IntraOpImagingWorkflowState(QState* parent);
  virtual ~IntraOpImagingWorkflowState(){};
  virtual QIcon getIcon() const { return QIcon(":/icons/workflow_state_acquisition.png"); }

  virtual void onEntry(QEvent* event);
  virtual bool canEnter() const;
};

class PostOpControllWorkflowState : public WorkflowState
{
  Q_OBJECT

public:
  PostOpControllWorkflowState(QState* parent);
  virtual ~PostOpControllWorkflowState(){};
  virtual QIcon getIcon() const { return QIcon(":/icons/workflow_state_post_op.png"); }

  virtual bool canEnter() const;
};

}
#endif /* CXWORKFLOWSTATE_H_ */
