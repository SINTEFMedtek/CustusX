/*
 * cxApplicationState.h
 *
 *  Created on: Aug 17, 2010
 *      Author: christiana
 */

#ifndef CXAPPLICATIONSTATE_H_
#define CXAPPLICATIONSTATE_H_


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

namespace cx
{
/**
 */

class ApplicationState : public QState
{
  Q_OBJECT

public:
  ApplicationState(QState* parent, QString uid, QString name) :
    QState(parent),
    mUid(uid), mName(name), mAction(NULL)
  {
  };

  virtual ~ApplicationState(){};

  virtual void onEntry(QEvent * event )
  {
    ssc::messageManager()->sendInfo("Application change to [" + string_cast(mName) + "]");
    if(mAction)
      mAction->setChecked(true);
  };
  virtual void onExit(QEvent * event )
  {
    //std::cout << "Exiting application "<< mName << std::endl;
  };

  virtual QString getUid() const {return mUid;};
  virtual QString getName() const {return mName;};

  std::vector<ApplicationState*> getChildStates()
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

  QAction* createAction(QActionGroup* group)
  {
    if(mAction)
      return mAction;

    mAction = new QAction(this->getName(), group);
    mAction->setCheckable(true);
    mAction->setData(QVariant(this->getUid()));
    //this->canEnterSlot();

    connect(mAction, SIGNAL(triggered()), this, SLOT(setActionSlot()));

    return mAction;
  };

protected slots:

  void setActionSlot()
  {
    this->machine()->postEvent(new RequestEnterStateEvent(this->getUid()));
  };

protected:

  QString mUid;
  QString mName;
  QAction* mAction;
};

class ParentApplicationState : public ApplicationState
{
  Q_OBJECT

public:
  ParentApplicationState(QState* parent) :
    ApplicationState(parent, "ParentUid", "Parent")
  {  };

  virtual void onEntry(QEvent * event )
  {
  };
  virtual void onExit(QEvent * event )
  {
  };

  virtual ~ParentApplicationState(){};
};

class LaboratoryApplicationState : public ApplicationState
{
  Q_OBJECT

public:
  LaboratoryApplicationState(QState* parent) :
    ApplicationState(parent, "Lab", "Laboratory")
  {  };
  virtual ~LaboratoryApplicationState(){};
};

class NeurologyApplicationState : public ApplicationState
{
  Q_OBJECT

public:
  NeurologyApplicationState(QState* parent) :
    ApplicationState(parent, "Nevro", "Neurology")
  {  };
  virtual ~NeurologyApplicationState(){};
};

class LaparascopyApplicationState : public ApplicationState
{
  Q_OBJECT

public:
  LaparascopyApplicationState(QState* parent) :
    ApplicationState(parent, "Lap", "Laparascopy")
  {
  };
  virtual ~LaparascopyApplicationState(){};
};



}


#endif /* CXAPPLICATIONSTATE_H_ */
