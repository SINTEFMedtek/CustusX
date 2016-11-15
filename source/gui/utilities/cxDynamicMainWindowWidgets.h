#ifndef CXDYNAMICMAINWINDOWWIDGETS_H_
#define CXDYNAMICMAINWINDOWWIDGETS_H_

#include <set>
#include <QString>
#include <QObject>
class QMainWindow;
class QDockWidget;
class QScrollArea;
class QWidget;
class QAction;
class QToolBar;
class QMenu;
class QActionGroup;
#include "cxStateService.h"

namespace cx
{
struct Desktop;

/**
 * \class DockWidgets
 * \ingroup cx_gui
 *
 * \brief Handles dock widgets for main window
 *
 * \date Sep 08, 2014
 * \author Ole Vegard Solberg, SINTEF
 */
class DynamicMainWindowWidgets : public QObject
{
	Q_OBJECT

public:
	DynamicMainWindowWidgets(QMainWindow* mainWindow);
	QDockWidget* addAsDockWidget(QWidget* widget, QString groupname, QObject* owningPlugin=NULL);
	void registerToolBar(QToolBar* toolbar, QString groupname = "Toolbars");
	void owningServiceRemoved(QObject* service);
	void hideAll();
	void restoreFrom(const Desktop& desktop);
	QMenu* createPopupMenu();
	void showWidget(QString name);

private slots:
	void onConsoleWindowTitleChanged(const QString & title);
	void onWidgetActionTriggered(bool checked);

private:
	/**
	 * Represent one widget inside a QMainWindow that can be dynamically
	 * added/removed, hidden/shown using the standard framework for
	 * QToolBars and QDockWidgets.
	 */
	struct DynamicWidget
	{
		QWidget* mWidget; ///< QToolBar or QDockWidget
		QString mGroup;
		QString mName;
		QObject* mOwningService; ///< the plugin object owning the widget, must follow lifetime of this plugin
	};
	void restorePreset(const Desktop::Preset& preset);

	QDockWidget* createDockWidget(QWidget* widget);
	QScrollArea *addVerticalScroller(QWidget* widget);
	void tabifyWidget(QDockWidget* dockWidget);
	typedef std::map<QString, QActionGroup*> ActionGroupMap;
	void addToWidgetGroupMap(ActionGroupMap& groups, QAction* action, QString groupname);
	QDockWidget* getFirstDockWidget(Qt::DockWidgetArea area);

	QMainWindow* mMainWindow;
	std::vector<DynamicWidget> mItems;
	QToolBar* mFirstDummyToolbar;
};

} // namespace cx

#endif // CXDYNAMICMAINWINDOWWIDGETS_H_
