#ifndef CXSTATESERVICE_H_
#define CXSTATESERVICE_H_

#include <QDomNode>

#include "cxForwardDeclarations.h"
#include "cxWorkflowStateMachine.h"
#include "cxApplicationStateMachine.h"
#include <QObject>

namespace cx
{

struct Desktop
{
  Desktop();
  Desktop(QString layout, QByteArray mainwindowstate);

  QString mLayoutUid;
  QByteArray mMainWindowState;
};

/**
 * \class cxStateManager.h
 *
 * \brief
 *
 * \date 4. aug. 2010
 * \author: Janne Beate Bakeng, SINTEF
 */
class StateService : public QObject
{
  Q_OBJECT

public:
  static StateService* getInstance(); ///< returns the only instance of this class
  static void destroyInstance();     ///< destroys the only instance of this class

  QString getVersionName();

  WorkflowStateMachinePtr getWorkflow();
  ApplicationStateMachinePtr getApplication();

  Desktop getActiveDesktop();
  void saveDesktop(Desktop desktop);
  void resetDesktop();

private:
  StateService();
  virtual ~StateService();

  void initialize(); ///< init stuff that is dependent of the statemanager
  void fillDefaultSettings();
  template<class T>
  void fillDefault(QString name, T value);

  static StateService* mTheInstance; ///< the only instance of this class

  WorkflowStateMachinePtr mWorkflowStateMachine;
  ApplicationStateMachinePtr mApplicationStateMachine;
};

StateService* stateService();

}
#endif /* CXSTATESERVICE_H_ */
