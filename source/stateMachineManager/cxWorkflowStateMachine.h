#ifndef CXWORKFLOWSTATEMACHINE_H_
#define CXWORKFLOWSTATEMACHINE_H_

#include <QStateMachine>
#include <QActionGroup>
#include "cxForwardDeclarations.h"

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

private:
  QAction* addAction(QString stateUid, QActionGroup* group);
  void newState(WorkflowState* state);

  typedef std::map<QString, WorkflowState*> WorkflowStateMap;
  WorkflowStateMap mStates;
};

typedef boost::shared_ptr<WorkflowStateMachine> WorkflowStateMachinePtr;

}
#endif /* CXWORKFLOWSTATEMACHINE_H_ */
