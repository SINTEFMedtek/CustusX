#ifndef CXStateManager_H_
#define CXStateManager_H_

#include <QDomNode>

#include "cxForwardDeclarations.h"
#include "cxWorkflowStateMachine.h"
#include "cxApplicationStateMachine.h"
#include "cxRecordSession.h"
#include <QObject>

namespace ssc
{
  typedef boost::shared_ptr<class Reconstructer> ReconstructerPtr;
}

namespace cx
{
typedef boost::shared_ptr<class RTSourceManager> IGTLinkConnectionPtr;

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
class StateManager : public QObject
{
  Q_OBJECT

public:
  static StateManager* getInstance(); ///< returns the only instance of this class
  static void destroyInstance();     ///< destroys the only instance of this class

  WorkflowStateMachinePtr getWorkflow();
  ApplicationStateMachinePtr getApplication();
  PatientDataPtr getPatientData();
  IGTLinkConnectionPtr getIGTLinkConnection();
  ssc::ReconstructerPtr getReconstructer();

  Desktop getActiveDesktop();
  void saveDesktop(Desktop desktop);
  void resetDesktop();

  void addRecordSession(RecordSessionPtr session);
  void removeRecordSession(RecordSessionPtr session);
  std::vector<RecordSessionPtr> getRecordSessions();
  RecordSessionPtr getRecordSession(QString uid);

  //Interface for saving/loading
  void addXml(QDomNode& dataNode); ///< adds xml information about the StateManager and its variabels
  void parseXml(QDomNode& dataNode);///< Use a XML node to load data. \param dataNode A XML data representation of the StateManager.

signals:
  void  recordedSessionsChanged();

private:
  StateManager();
  ~StateManager();

  void initialize(); ///< init stuff that is dependent of the statemanager
  void fillDefaultSettings();
  template<class T>
  void fillDefault(QString name, T value);

  static StateManager* mTheInstance; ///< the only instance of this class

  WorkflowStateMachinePtr mWorkflowStateMachine;
  ApplicationStateMachinePtr mApplicationStateMachine;
  PatientDataPtr mPatientData;
  IGTLinkConnectionPtr mIGTLinkConnection;
  ssc::ReconstructerPtr mReconstructer;

  std::vector<RecordSessionPtr> mRecordSessions;
};
StateManager* stateManager();
}
#endif /* CXStateManager_H_ */
