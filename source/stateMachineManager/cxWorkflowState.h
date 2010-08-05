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

  virtual void onEntry(QEvent * event ){std::cout << "Entering "<< mName << std::endl;};
  virtual void onExit(QEvent * event ){std::cout << "Exiting "<< mName << std::endl;};

  virtual QString getUid() const {return mUid;};
  virtual QString getName() const {return mName;};

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
    connect(ssc::dataManager(), SIGNAL(dataLoaded()), this, SLOT(canEnterSlot()));
  };

  virtual ~NavigationWorkflowState(){};

  virtual bool canEnter() const {return !ssc::dataManager()->getImages().empty();};
};

}
#endif /* CXWORKFLOWSTATE_H_ */
