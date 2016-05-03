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

#include "cxGuiExport.h"

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
class DynamicMainWindowWidgets;
class MainWindowActions;

typedef boost::shared_ptr<class CameraControl> CameraControlPtr;
typedef boost::shared_ptr<class LayoutInteractor> LayoutInteractorPtr;
typedef boost::shared_ptr<class VisServices> VisServicesPtr;
typedef boost::shared_ptr<class ProcessWrapper> ProcessWrapperPtr;


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
class cxGui_EXPORT MainWindow: public QMainWindow
{
	Q_OBJECT

public:
	MainWindow();
	virtual ~MainWindow();

	virtual QMenu* createPopupMenu();

protected slots:
	void patientChangedSlot();
	void delayedShow();

	//application menu
	void aboutSlot();
	void preferencesSlot();
	void toggleFullScreenSlot();

	void showControlPanelActionSlot();
	void showSecondaryViewLayoutWindowActionSlot();

	// application
	void onApplicationStateChangedSlot();

	//workflow
	void onWorkflowStateChangedSlot();
	void saveDesktopSlot();
	void resetDesktopSlot();

	// help
	void onShowContextSentitiveHelp();

	void dockWidgetVisibilityChanged(bool val);
	void focusChanged(QWidget * old, QWidget * now);

	void onGUIExtenderServiceAdded(GUIExtenderService* service);
	void onGUIExtenderServiceRemoved(GUIExtenderService* service);
	void onGUIExtenderServiceModified(GUIExtenderService* service);

protected:
	void changeEvent(QEvent * event);

private:
	void focusInsideDockWidget(QObject* dockWidget);
	LayoutInteractorPtr mLayoutInteractor;
	void updateWindowTitle();
	void createActions(); ///< creates and connects (gui-)actions
	void createMenus(); ///< creates and add (gui-)menues
	void createToolBars(); ///< creates and adds toolbars for convenience

	QToolBar *registerToolBar(QString name, QString groupname="Toolbars");
	void setupGUIExtenders();

	void closeEvent(QCloseEvent *event);///< Save geometry and window state at close
	QDockWidget* addAsDockWidget(QWidget* widget, QString groupname);

	//menus
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
	QAction* mFullScreenAction;
	QAction* mQuitAction;

	QAction* mShowControlPanelAction;
	QAction* mSecondaryViewLayoutWindowAction;

	QActionGroup* mStandard3DViewActions; ///< actions for setting camera in fixed direction.

	QAction* mShowContextSensitiveHelpAction;
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

	boost::shared_ptr<ServiceTrackerListener<GUIExtenderService> > mServiceListener;

	//widgets
	QPointer<class SecondaryMainWindow> mControlPanel;
	QPointer<class SecondaryViewLayoutWindow> mSecondaryViewLayoutWindow;

	DynamicMainWindowWidgets* mDockWidgets;
	MainWindowActions* mActions;

	VisServicesPtr mServices;
};

}//namespace cx

#endif /* CXMAINWINDOW_H_ */
