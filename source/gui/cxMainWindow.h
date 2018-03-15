/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
	QMap<QString, QPointer<class SecondaryMainWindow> > mSecondaryMainWindows;
	QMap<QString, QPointer<QAction> > mSecondaryMainWindowsActions;

	DynamicMainWindowWidgets* mDockWidgets;
	MainWindowActions* mActions;

	VisServicesPtr mServices;
	void createActionForWidgetInSeparateWindow(QWidget *widget);
};

}//namespace cx

#endif /* CXMAINWINDOW_H_ */
