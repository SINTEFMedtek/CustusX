#ifndef CXMAINWINDOW_H_
#define CXMAINWINDOW_H_

#include <QMainWindow>
#include "sscData.h"
#include "sscConsoleWidget.h"
#include "cxViewManager.h"
#include "cxStateMachineManager.h"

class QAction;
class QMenu;
class QActionGroup;

namespace ssc
{
  class ReconstructionWidget;
}

namespace cx
{
typedef boost::shared_ptr<class CameraControl> CameraControlPtr;

/**
 * \class MainWindow
 *
 * \brief This is the main gui class which controls the workflow.
 *
 * \date Jan 20, 2009
 * \author Janne Beate Bakeng, SINTEF
 * \author Ole Vegard Solberg, SINTEF
 */
class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  MainWindow(); ///< sets up the initial gui
  ~MainWindow(); ///< empty
  
  static void initialize();
  static void shutdown(); ///< deallocate all global resources. Assumes MainWindow already has been destroyed and the mainloop is exited

  virtual QMenu* createPopupMenu();

//signals:
//  void deleteCurrentImage(); ///< Sends a signal when the current image is to be deleted

protected slots:
  void patientChangedSlot();
  //application menu
  void aboutSlot();
  void preferencesSlot();
  void quitSlot();
  void toggleFullScreenSlot();

  // File menu
  void newPatientSlot(); ///< Create new patient with directory structure
  void loadPatientFileSlot();///< Load all application data from XML file
  void savePatientFileSlot();///< Save all application data to XML file
  void clearPatientSlot();///< clear current patient (debug)

  void showControlPanelActionSlot();

  // application
  void onApplicationStateChangedSlot();

  //workflow
  void onWorkflowStateChangedSlot();
  void saveDesktopSlot();
  void resetDesktopSlot();

  //data menu
  void importDataSlot(); ///< loads data(images) into the datamanager
  void deleteDataSlot(); ///< deletes data(image) from the patient

  void togglePointPickerActionSlot();
  void updatePointPickerActionSlot();

  //tool menu
  void configureSlot(); ///< lets the user choose which configuration files to use for the navigation
  void manualToolPhysicalPropertiesSlot();

  // layout menu
  void layoutChangedSlot();
  void newCustomLayoutSlot();
  void editCustomLayoutSlot();
  void deleteCustomLayoutSlot();

  // navigation
  void centerToImageCenterSlot();
  void centerToTooltipSlot();

  void updateTrackingActionSlot();
  void toggleTrackingSlot();
  void toggleStreamingSlot();
  void updateStreamingActionSlot();

  void shootScreen();
  void shootWindow();

protected:
  void changeEvent(QEvent * event);

private:
  void saveScreenShot(QPixmap pixmap);
  void updateWindowTitle();
  void createActions(); ///< creates and connects (gui-)actions
  void createMenus(); ///< creates and add (gui-)menues
  void createToolBars(); ///< creates and adds toolbars for convenience
  void createStatusBar();  ///<

  void addAsDockWidget(QWidget* widget, QString groupname = "");
  void registerToolBar(QToolBar* toolbar, QString groupname="");
  void addToWidgetGroupMap(QAction* action, QString groupname);
  void updateManualToolPhysicalProperties();

  LayoutData executeLayoutEditorDialog(QString title, bool createNew);
  void startupLoadPatient();

  void closeEvent(QCloseEvent *event);///< Save geometry and window state at close

  //gui
  QWidget* mCentralWidget; ///< central widget used for views

  //menus
  QMenu* mCustusXMenu;    ///< Application menu
  QMenu* mFileMenu;       ///< Menu for file operations (ex: save/load)
  QMenu* mWorkflowMenu;   ///< menu for choosing workflow
  QMenu* mToolMenu;       ///< menu for interacting with the navigation system
  QMenu* mLayoutMenu;     ///< menu for changing view layouts
  QMenu* mNavigationMenu; ///< menu for navigation and interaction
  QMenu* mHelpMenu;

  QAction* mShowControlPanelAction;

  //actions and actiongroups
  QAction* mAboutAction;
  QAction* mPreferencesAction;
  QAction* mDebugModeAction;
  QAction* mFullScreenAction;
  QAction* mQuitAction;
  
  QAction* mShootScreenAction;
  QAction* mShootWindowAction;

  QAction* mNewPatientAction;///< Action for creating a new patient
  QAction* mLoadFileAction;///< Action for loading all data from file
  QAction* mSaveFileAction;///< Action for saving all data to file
  QAction* mClearPatientAction;
  QAction* mExportPatientAction;

  QActionGroup* mStandard3DViewActions; ///< actions for setting camera in fixed direction.
  QAction* mShowPointPickerAction;

  QAction* mImportDataAction; ///< action for loading data into the datamanager
  QAction* mDeleteDataAction; ///< action for deleting the current volume

  QAction* mManualToolPhysicalProperties;
  QAction* mConfigureToolsAction; ///< action for configuring the toolmanager
  QAction* mInitializeToolsAction; ///< action for initializing contact with the navigation system
  QAction* mTrackingToolsAction; ///< action for asking the navigation system to start/stop tracking
  QAction* mSaveToolsPositionsAction; ///< action for saving the tool positions
  QAction* mStartStreamingAction; ///< start streaming of the default RT source.
  QActionGroup* mToolsActionGroup; ///< grouping the actions for contacting the navigation system

  QActionGroup* mLayoutActionGroup; ///< grouping the view layout actions
  QAction* mNewLayoutAction; ///< create a new custom layout
  QAction* mEditLayoutAction; ///< edit the current custom layout
  QAction* mDeleteLayoutAction; ///< delete the current custom layout

  // actions for image navigation
  QAction* mCenterToImageCenterAction;
  QAction* mCenterToTooltipAction;
  QActionGroup* mInteractorStyleActionGroup;

  //desktop actions
  QAction* mSaveDesktopAction;
  QAction* mResetDesktopAction;

  //toolbars
  QToolBar* mDataToolBar; ///< toolbar for data actions
  QToolBar* mToolToolBar; ///< toolbar for tracking system actions
  QToolBar* mNavigationToolBar; ///< toolbar for navigation actions
  QToolBar* mInteractorStyleToolBar; ///< toolbar for camera interaction styles
  QToolBar* mWorkflowToolBar; ///< toolbar for workflow actions
  QToolBar* mDesktopToolBar; ///< toolbar for desktop actions
  QToolBar* mHelpToolBar; ///< toolbar for entering help mode
  QToolBar* mScreenshotToolBar;

  std::map<QString, QActionGroup*> mWidgetGroupsMap; ///< map containing groups

  //widgets
  ssc::ConsoleWidget*                           mConsoleWidget;
  class RegistrationMethodsWidget*              mRegsitrationMethodsWidget; ///< container widget for all registrations
  class SegmentationMethodsWidget*              mSegmentationMethodsWidget; ///< container widget for all segmentation methods
  class VisualizationMethodsWidget*             mVisualizationMethodsWidget; ///< container widget for all visualization methods/filters
  class CalibrationMethodsWidget*               mCalibrationMethodsWidget; ///< container widget for all calibration methods
  class BrowserWidget*                          mBrowserWidget; ///< contains tree structure with the images, meshes and tools
  class NavigationWidget*                       mNavigationWidget; ///< contains settings for navigating
  class ImagePropertiesWidget*                  mImagePropertiesWidget; ///< display and control of image properties for active image.
  class ToolPropertiesWidget*                   mToolPropertiesWidget; ///< display and control of tool properties for active tool.
  class MeshPropertiesWidget*                   mMeshPropertiesWidget; ///< Display and control image properties for active mesh
  class PointSamplingWidget*                    mPointSamplingWidget;
  ssc::ReconstructionWidget*                    mReconstructionWidget;
  class RegistrationHistoryWidget*              mRegistrationHistoryWidget; ///< look back in registration history.
  class VolumePropertiesWidget*                 mVolumePropertiesWidget;
  class StatusBar*                        mCustomStatusBar;
  class FrameTreeWidget*                        mFrameTreeWidget;
  class SecondaryMainWindow*                           mControlPanel;

  //Preferences
  CameraControlPtr mCameraControl;
  std::set<QDockWidget*> mDockWidgets;
};
}//namespace cx

#endif /* CXMAINWINDOW_H_ */
