#ifndef CXDOCKWIDGETS_H
#define CXDOCKWIDGETS_H

#include <set>
#include <QString>
#include <QObject>
class QMainWindow;
class QDockWidget;
class QScrollArea;
class QWidget;
class QAction;

namespace cx
{

/**
 * \class DockWidgets
 * \ingroup cx_gui
 *
 * \brief Handles dock widgets for main window
 *
 * \date Sep 08, 2014
 * \author Ole Vegard Solberg, SINTEF
 */
class DockWidgets : public QObject
{
	Q_OBJECT
public:
	DockWidgets(QMainWindow* parent);
	QDockWidget* addAsDockWidget(QWidget* widget, QString groupname = "");
	void erase(QDockWidget* dockWidget);
	void toggleDebug(QAction* action, bool checked);
	void hideAll();
private slots:
	void onConsoleWindowTitleChanged(const QString & title);
private:
	QDockWidget* createDockWidget(QWidget* widget);
	QScrollArea *addVerticalScroller(QWidget* widget);
	void tabifyWidget(QDockWidget* dockWidget);

	QMainWindow* mParent;
	std::set<QDockWidget*> mDockWidgets;
};
} // namespace cx
#endif // CXDOCKWIDGETS_H
