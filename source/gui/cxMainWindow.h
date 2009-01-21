#ifndef CXMAINWINDOW_H_
#define CXMAINWINDOW_H_

#include <QMainWindow>
//#include <boost/shared_ptr.hpp>

/**
 * cxMainWindow.h
 *
 * \brief
 *
 * \date Jan 20, 2009
 * \author: jbake
 */

class QAction;
class QMenu;
class QActionGroup;

namespace ssc
{
class DataManager;
}

namespace cx
{
typedef ssc::DataManager DataManager;

class RepManager;
class ViewManager;
class ToolManager;

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  MainWindow();
  ~MainWindow();

private slots:
  //application menu
  void aboutSlot();
  void preferencesSlot();
  void quitSlot();

  //workflow menu
  void patientDataWorkflowSlot();
  void imageRegistrationWorkflowSlot();
  void patientRegistrationWorkflowSlot();
  void navigationWorkflowSlot();
  void usAcquisitionWorkflowSlot();

  //data menu
  void loadDataSlot();


private:
  void createActions();
  void createMenus();
  void createToolBars();
  void createStatusBar();

  ViewManager* mViewManager;
  DataManager* mDataManager;
  ToolManager* mToolManager;
  RepManager* mRepManager;

  QWidget* mCentralWidget;

  QMenu* mWorkflowMenu;
  QMenu* mDataMenu;
  QMenu* mToolMenu;
  QMenu* mLayoutMenu;

  QAction* mPatientDataWorkflowAction;
  QAction* mImageRegistrationWorkflowAction;
  QAction* mPatientRegistrationWorkflowAction;
  QAction* mNavigationWorkflowAction;
  QAction* mUSAcquisitionWorkflowAction;
  QActionGroup* mWorkflowActionGroup;

  QAction* mLoadDataAction;

  QAction* mConfigureToolsAction;
  QAction* mInitializeToolsAction;
  QAction* mStartTrackingToolsAction;
  QAction* mStopTrackingToolsAction;
  QActionGroup* mToolsActionGroup;

  QAction* m3D_1x1_LayoutAction;
  QAction* m3DACS_2x2_LayoutAction;
  QAction* m3DACS_1x3_LayoutAction;
  QAction* mACSACS_2x3_LayoutAction;
  QActionGroup* mLayoutActionGroup;

  QToolBar* mDataToolBar; //load
  QToolBar* mToolToolBar; //start and stop

  //cxImageRegistrationDockWidget* mImageRegistrationDockWidget; //TODO
  //cxPatientRegistrationDockWidget* mPatientRegistrationDockWidget; //TODO
  //cxVolumetricTFsDockWidget* mVolumetricTFsDockWidget; //TODO
  //cxStatusBarWidget* mStatusBarWidget; //TODO

  //Prefrences
  QString mCurrentPatientDataFolder;
  QString mCurrentToolConfigFile;

};
}//namespace cx

#endif /* CXMAINWINDOW_H_ */
