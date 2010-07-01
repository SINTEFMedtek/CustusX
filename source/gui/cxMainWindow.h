#ifndef CXMAINWINDOW_H_
#define CXMAINWINDOW_H_

#include <QMainWindow>
#include "sscData.h"
#include "cxViewManager.h"

class QAction;
class QMenu;
class QActionGroup;
class QDomDocument;
//class QSettings;
typedef boost::shared_ptr<class QSettings> QSettingsPtr;

namespace ssc
{
  class ReconstructionWidget;
}


namespace cx
{
typedef boost::shared_ptr<class PatientData> PatientDataPtr;

class CustomStatusBar;
class ContextDockWidget;
class TransferFunctionWidget;
class BrowserWidget;
class NavigationWidget;
class ImageRegistrationWidget;
class PatientRegistrationWidget;
class ShiftCorrectionWidget;


 
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
  
signals:
  void deleteCurrentImage(); ///< Sends a signal when the current image is to be deleted

protected slots:
  void patientChangedSlot();
  //application menu
  void aboutSlot(); ///< TODO
  void preferencesSlot();
  void quitSlot(); ///< TODO
  
  // File menu
  void newPatientSlot(); ///< Create new patient with directory structure
  void loadPatientFileSlot();///< Load all application data from XML file
  void savePatientFileSlot();///< Save all application data to XML file

  //workflow menu
  void patientDataWorkflowSlot(); ///< change state to patient data
  void imageRegistrationWorkflowSlot(); ///< change state to image registration
  void patientRegistrationWorkflowSlot(); ///< change state to patient registration
  void navigationWorkflowSlot(); ///< change state to navigation
  void usAcquisitionWorkflowSlot(); ///< change state to ultrasound acquisition

  //data menu
  void importDataSlot(); ///< loads data(images) into the datamanager
  void deleteDataSlot(); ///< deletes data(image) from the patient
  void loadPatientRegistrationSlot(); ///< loads a patient registration

  //tool menu
  void configureSlot(); ///< lets the user choose which configuration files to use for the navigation

  // layout menu
  void layoutChangedSlot();
  void setLayoutSlot();

  //logging
  void loggingSlot(const QString& message, int timeout); ///< prints messages for debugging

  // navigation
  void centerToImageCenterSlot();
  void centerToTooltipSlot();

protected:
  enum WorkflowState
  {
    PATIENT_DATA,
    IMAGE_REGISTRATION,
    PATIENT_REGISTRATION,
    NAVIGATION,
    US_ACQUISITION
  }; ///< the different workflow(-states) the user can choose from

  void createActions(); ///< creates and connects (gui-)actions
  void createMenus(); ///< creates and add (gui-)menues
  void createToolBars(); ///< creates and adds toolbars for convenience
  void createStatusBar();  ///< //TODO

  void addAsDockWidget(QWidget* widget);

  //Takes care of removing and adding widgets depending on which workflow state the system is in
  void changeState(WorkflowState fromState, WorkflowState toState); ///< used to change state
  void activatePatientDataState(); ///< Should only be used by changeState(...)!
  void deactivatePatientDataState(); ///< Should only be used by changeState(...)!
  void activateImageRegistationState(); ///< Should only be used by changeState(...)!
  void deactivateImageRegistationState(); ///< Should only be used by changeState(...)!
  void activatePatientRegistrationState(); ///< Should only be used by changeState(...)!
  void deactivatePatientRegistrationState(); ///< Should only be used by changeState(...)!
  void activateNavigationState(); ///< Should only be used by changeState(...)!
  void deactivateNavigationState(); ///< Should only be used by changeState(...)!
  void activateUSAcquisitionState(); ///< Should only be used by changeState(...)!
  void deactivateUSAcquisitionState(); ///< Should only be used by changeState(...)!
  
//  void createPatientFolders(QString choosenDir); ///< Create patient folders and save xml for new patient and for load patient for a directory whitout xml file.
  QAction* addLayoutAction(LayoutType layout);

  void closeEvent(QCloseEvent *event);///< Save geometry and window state at close
  
  WorkflowState mCurrentWorkflowState; ///< the current workflow in use

  //gui
  QWidget* mCentralWidget; ///< central widget used for views

  //menus
  QMenu* mCustusXMenu; ///< Application menu
  QMenu* mFileMenu; ///< Menu for file operations (ex: save/load)
  QMenu* mWindowMenu; ///< Menu for showing / hiding GUI elements
  QMenu* mWorkflowMenu; ///< menu for choosing workflow
  QMenu* mDataMenu; ///< menu for loading data
  QMenu* mToolMenu; ///< menu for interacting with the navigation system
  QMenu* mLayoutMenu; ///< menu for changing view layouts

  //actions and actiongroups
  QAction* mAboutAction;
  QAction* mPreferencesAction;
  QAction* mDebugModeAction;
  QAction* mQuitAction;
  
  QAction* mNewPatientAction;///< Action for creating a new patient
  QAction* mLoadFileAction;///< Action for loading all data from file
  QAction* mSaveFileAction;///< Action for saving all data to file
	
  QAction* mToggleContextDockWidgetAction;///< Action for turning dock widget on/off
  
  QAction* mPatientDataWorkflowAction; ///< action for switching to the patient data workflow
  QAction* mImageRegistrationWorkflowAction; ///< action for switching to the image registration workflow
  QAction* mPatientRegistrationWorkflowAction; ///< action for switching to the patient registraiton workflow
  QAction* mNavigationWorkflowAction; ///< action for switching to the navigation workflow
  QAction* mUSAcquisitionWorkflowAction; ///< action for switching to the ultrasound acqusition workflow
  QActionGroup* mWorkflowActionGroup; ///< grouping the workflow actions

  QAction* mImportDataAction; ///< action for loading data into the datamanager
  QAction* mDeleteDataAction; ///< action for deleting the current volume
  QAction* mLoadPatientRegistrationFromFile; ///< action for loading a patient registration from file

  QAction* mConfigureToolsAction; ///< action for configuring the toolmanager
  QAction* mInitializeToolsAction; ///< action for initializing contact with the navigation system
  QAction* mStartTrackingToolsAction; ///< action for asking the navigation system to start tracking
  QAction* mStopTrackingToolsAction; ///< action for asking the navigation system to stop tracking
  QAction* mSaveToolsPositionsAction; ///< action for saving the tool positions
  QActionGroup* mToolsActionGroup; ///< grouping the actions for contacting the navigation system

//  QAction* m3D_1x1_LayoutAction; ///< action for switching to 3D_1x1 view layout
//  QAction* m3DACS_2x2_LayoutAction; ///< action for switching to 3DACS_2x2 view layout
//  QAction* m3DACS_1x3_LayoutAction; ///< action for switching to 3DACS_1x3 view layout
//  QAction* mACSACS_2x3_LayoutAction; ///< action for switching to ACSACS_2x3 view layout
  QActionGroup* mLayoutActionGroup; ///< grouping the view layout actions

  // actions for image navigation
  QAction* mCenterToImageCenterAction;
  QAction* mCenterToTooltipAction;

  //toolbars
  QToolBar* mDataToolBar; ///< toolbar for data actions
  QToolBar* mToolToolBar; ///< toolbar for tracking system actions
  QToolBar* mNavigationToolBar; ///< toolbar for navigation actions

  ContextDockWidget*          mContextDockWidget; ///< dock widget for context sensitive widgets
  ImageRegistrationWidget*    mImageRegistrationWidget; ///< interface for image registration
  PatientRegistrationWidget*  mPatientRegistrationWidget; ///< interface for patient registration
  TransferFunctionWidget*     mTransferFunctionWidget; ///< interface for changing a images transfere function
  ShiftCorrectionWidget*      mShiftCorrectionWidget; ///< interface for image shift correction
  BrowserWidget*              mBrowserWidget; ///< contains tree structure with the images, meshes and tools
  NavigationWidget*           mNavigationWidget; ///< contains settings for navigating
  CustomStatusBar*            mCustomStatusBar; //TODO, needs some work
  class ImagePropertiesWidget* mImagePropertiesWidget; ///< display and control of image properties for active image.
  class ToolPropertiesWidget* mToolPropertiesWidget; ///< display and control of tool properties for active tool.
  class PointSamplingWidget* mPointSamplingWidget;
  ssc::ReconstructionWidget* mReconstructionWidget;
  class RegistrationHistoryWidget* mRegistrationHistoryWidget; ///< look back in registration history.
  int mImageRegistrationIndex, mShiftCorrectionIndex, mPatientRegistrationIndex, mNavigationIndex; ///< tab index for removing tabs is ContextDockWidget

  //Preferences
  QSettingsPtr mSettings; ///< Object for storing all program/user specific settings

  PatientDataPtr mPatientData;
  
};
}//namespace cx

#endif /* CXMAINWINDOW_H_ */
