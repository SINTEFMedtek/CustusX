/*
 * cxApplicationStateMachine.h
 *
 *  Created on: Aug 17, 2010
 *      Author: christiana
 */

#ifndef CXAPPLICATIONSTATEMACHINE_H_
#define CXAPPLICATIONSTATEMACHINE_H_

#include <QStateMachine>
#include <QActionGroup>
#include "cxForwardDeclarations.h"

class QToolBar;
class QMenu;

namespace cx
{

class ApplicationState;

/**
 */
class ApplicationStateMachine : public QStateMachine
{
  Q_OBJECT
public:
  ApplicationStateMachine();
  virtual ~ApplicationStateMachine();

  QActionGroup* getActionGroup();

  QString getActiveUidState();
  QString getActiveStateName();

  QStringList getAllApplicationNames();

signals:
  void activeStateChanged();

private slots:
  void activeStateChangedSlot();

private:
  QAction* addAction(QString stateUid, QActionGroup* group);
  ApplicationState* newState(ApplicationState* state);

  typedef std::map<QString, ApplicationState*> ApplicationStateMap;
  ApplicationStateMap mStates;
  ApplicationState* mParentState;
  QActionGroup* mActionGroup;
};

typedef boost::shared_ptr<ApplicationStateMachine> ApplicationStateMachinePtr;

}


#endif /* CXAPPLICATIONSTATEMACHINE_H_ */
