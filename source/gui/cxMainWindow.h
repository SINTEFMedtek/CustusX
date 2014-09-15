/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
=========================================================================*/

#ifndef CXMAINWINDOW_H_
#define CXMAINWINDOW_H_

#include <QMainWindow>
#include <map>
#include <set>
#include "boost/shared_ptr.hpp"
#include "cxGUIExtenderService.h"
#include <QPointer>
#include "cxServiceTrackerListener.h"

class QAction;
class QMenu;
class QActionGroup;
class QDockWidget;
class QScrollArea;

namespace cx
{
class LayoutData;
class GUIExtenderService;
class ConsoleWidget;
class DockWidgets;
typedef boost::shared_ptr<class CameraControl> CameraControlPtr;
typedef boost::shared_ptr<class LayoutInteractor> LayoutInteractorPtr;

/**
 * \class MainWindow
 * \ingroup cx_gui
 *
 * \brief This is the main gui class which controls the workflow.
 *
 * \date Jan 20, 2009
 * \author Janne Beate Bakeng, SINTEF
 * \author Ole Vegard Solberg, SINTEF
 * \author Christian Askeland, SINTEF
 */
class MainWindow: public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(std::vector<GUIExtenderServicePtr> guiExtenders);
	virtual ~MainWindow();

	virtual QMenu* createPopupMenu();

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
	void showSecondaryViewLayoutWindowActionSlot();

	// application
	void onApplicationStateChangedSlot();

	//workflow
	void onWorkflowStateChangedSlot();
	void saveDesktopSlot();
	void resetDesktopSlot();

	//data menu
	void exportDataSlot();
	void importDataSlot(); ///< loads data(images) into the datamanager
	void deleteDataSlot(); ///< deletes data(image) from the patient

	void togglePointPickerActionSlot();
	void updatePointPickerActionSlot();

	//tool menu
	void configureSlot(); ///< lets the user choose which configuration files to use for the navigation

	// navigation
	void centerToImageCenterSlot();
	void centerToTooltipSlot();

	void updateTrackingActionSlot();
	void toggleTrackingSlot();
	void toggleStreamingSlot();
	void updateStreamingActionSlot();

	void shootScreen();
	void shootWindow();
	void recordFullscreen();

	//debug mode
	void toggleDebugModeSlot(bool checked);

	void startupLoadPatient();

    void onPluginBaseAdded(GUIExtenderService* service);
    void onPluginBaseRemoved(GUIExtenderService* service);
	void onPluginBaseModified(GUIExtenderService* service);

protected:
	void changeEvent(QEvent * event);

private:
	LayoutInteractorPtr mLayoutInteractor;
	void saveScreenShot(QPixmap pixmap);
	void saveScreenShotThreaded(QImage pixmap, QString filename);
	void updateWindowTitle();
	void createActions(); ///< creates and connects (gui-)actions
	void createMenus(); ///< creates and add (gui-)menues
	void createToolBars(); ///< creates and adds toolbars for convenience

	void registerToolBar(QToolBar* toolbar, QString groupname = "");
	void addToWidgetGroupMap(QAction* action, QString groupname);
	void addGUIExtender(GUIExtenderService* service);
	QWidget *addCategorizedWidget(GUIExtenderService::CategorizedWidget categorizedWidget);
	void removeGUIExtender(GUIExtenderService* service);
	void setupGUIExtenders();

	void closeEvent(QCloseEvent *event);///< Save geometry and window state at close
	QDockWidget* addAsDockWidget(QWidget* widget, QString groupname);

	//menus
	QMenu* mCustusXMenu; ///< Application menu
	QMenu* mFileMenu; ///< Menu for file operations (ex: save/load)
	QMenu* mWorkflowMenu; ///< menu for choosing workflow
	QMenu* mToolMenu; ///< menu for interacting with the navigation system
	QMenu* mLayoutMenu; ///< menu for changing view layouts
	QMenu* mNavigationMenu; ///< menu for navigation and interaction
	QMenu* mHelpMenu;
	QAction* mHelpMenuAction; ///< Action for helpMenu

	//actions and actiongroups
	QAction* mAboutAction;
	QAction* mPreferencesAction;
	QAction* mDebugModeAction;
	QAction* mFullScreenAction;
	QAction* mQuitAction;

	QAction* mShootScreenAction;
	QAction* mShootWindowAction;
	QAction* mRecordFullscreenAction;

	QAction* mNewPatientAction;///< Action for creating a new patient
	QAction* mLoadFileAction;///< Action for loading all data from file
	QAction* mSaveFileAction;///< Action for saving all data to file
	QAction* mClearPatientAction;
	QAction* mExportPatientAction;
	QAction* mShowControlPanelAction;
	QAction* mSecondaryViewLayoutWindowAction;

	QActionGroup* mStandard3DViewActions; ///< actions for setting camera in fixed direction.
	QAction* mShowPointPickerAction;

	QAction* mImportDataAction; ///< action for loading data into the datamanager
	QAction* mDeleteDataAction; ///< action for deleting the current volume

	QAction* mConfigureToolsAction; ///< action for configuring the toolmanager
	QAction* mInitializeToolsAction; ///< action for initializing contact with the navigation system
	QAction* mTrackingToolsAction; ///< action for asking the navigation system to start/stop tracking
	QAction* mSaveToolsPositionsAction; ///< action for saving the tool positions
	QAction* mStartStreamingAction; ///< start streaming of the default RT source.
	QActionGroup* mToolsActionGroup; ///< grouping the actions for contacting the navigation system

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

	QString mLastImportDataFolder;

	boost::shared_ptr<ServiceTrackerListener<GUIExtenderService> > mServiceListener;
	std::map<GUIExtenderService*, std::vector<QWidget*> > mWidgetsByPlugin;

	//widgets
	QPointer<class SecondaryMainWindow> mControlPanel;
	QPointer<class SecondaryViewLayoutWindow> mSecondaryViewLayoutWindow;

	//Preferences
	CameraControlPtr mCameraControl;

	DockWidgets* mDockWidgets;
};

}//namespace cx

#endif /* CXMAINWINDOW_H_ */
