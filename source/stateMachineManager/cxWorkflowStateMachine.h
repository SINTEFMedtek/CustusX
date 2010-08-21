#ifndef CXWORKFLOWSTATEMACHINE_H_
#define CXWORKFLOWSTATEMACHINE_H_

#include <QStateMachine>
#include <QActionGroup>
#include "cxForwardDeclarations.h"

class QToolBar;
class QMenu;

namespace cx
{

class WorkflowState;

/**
 * \class WorkflowStateMachine
 *
 * \brief
 *
 * \date 4. aug. 2010
 * \author: Janne Beate Bakeng, SINTEF
 */
class WorkflowStateMachine : public QStateMachine
{
  Q_OBJECT
public:
  WorkflowStateMachine();
  virtual ~WorkflowStateMachine();

  QActionGroup* getActionGroup();
  void fillMenu(QMenu* menu);
  void fillToolBar(QToolBar* toolbar);

  QString getActiveUidState();
  void setActiveState(QString uid);

signals:
  void activeStateChanged();

private:
  void fillMenu(QMenu* menu, WorkflowState* current);
  void fillToolbar(QToolBar* toolbar, WorkflowState* current);


  QAction* addAction(QString stateUid, QActionGroup* group);
  WorkflowState* newState(WorkflowState* state);

  typedef std::map<QString, WorkflowState*> WorkflowStateMap;
  WorkflowStateMap mStates;
  WorkflowState* mParentState;
  QActionGroup* mActionGroup;
};

typedef boost::shared_ptr<WorkflowStateMachine> WorkflowStateMachinePtr;

}
#endif /* CXWORKFLOWSTATEMACHINE_H_ */
