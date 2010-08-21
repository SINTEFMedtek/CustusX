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
#include "sscDataManager.h"
#include "cxStateMachineManager.h"
#include "cxPatientData.h"

namespace cx
{

/**
 */
class ApplicationState : public QState
{
  Q_OBJECT
public:
  ApplicationState(QState* parent, QString uid, QString name);
  virtual ~ApplicationState();
  virtual void onEntry(QEvent * event );
  virtual void onExit(QEvent * event );
  virtual QString getUid() const;
  virtual QString getName() const;
  std::vector<ApplicationState*> getChildStates();
  QAction* createAction(QActionGroup* group);
  virtual ssc::MEDICAL_DOMAIN getMedicalDomain() const = 0;
protected slots:
  void setActionSlot();
protected:
  QString mUid;
  QString mName;
  QAction* mAction;
  bool mActive;
};

class ParentApplicationState : public ApplicationState
{
  Q_OBJECT
public:
  ParentApplicationState(QState* parent) : ApplicationState(parent, "ParentUid", "Parent") {}
  virtual void onEntry(QEvent * event ) {}
  virtual void onExit(QEvent * event ) {}
  virtual ~ParentApplicationState(){};
  virtual ssc::MEDICAL_DOMAIN getMedicalDomain() const { return ssc::mdCOUNT; }
};

class LaboratoryApplicationState : public ApplicationState
{
  Q_OBJECT
public:
  LaboratoryApplicationState(QState* parent) : ApplicationState(parent, "Lab", "Laboratory") {}
  virtual ~LaboratoryApplicationState(){};
  virtual ssc::MEDICAL_DOMAIN getMedicalDomain() const { return ssc::mdLABORATORY; }
};

class NeurologyApplicationState : public ApplicationState
{
  Q_OBJECT
public:
  NeurologyApplicationState(QState* parent) : ApplicationState(parent, "Nevro", "Neurology") {}
  virtual ~NeurologyApplicationState() {}
  virtual ssc::MEDICAL_DOMAIN getMedicalDomain() const { return ssc::mdNEUROLOGY; }
};

class LaparascopyApplicationState : public ApplicationState
{
  Q_OBJECT
public:
  LaparascopyApplicationState(QState* parent) : ApplicationState(parent, "Lap", "Laparascopy") {}
  virtual ~LaparascopyApplicationState() {}
  virtual ssc::MEDICAL_DOMAIN getMedicalDomain() const { return ssc::mdLAPAROSCOPY; }
};

} // namespace cx


#endif /* CXAPPLICATIONSTATE_H_ */
