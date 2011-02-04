#ifndef SSCMAINWINDOW_H_
#define SSCMAINWINDOW_H_

#include <QtGui>

namespace ssc
{

class MainWindow : public QMainWindow
{
	Q_OBJECT
public:
	MainWindow(QWidget* parent = 0);
	~MainWindow() {}
	QToolBar* mToolbar;
	QPlainTextEdit* mTextEdit;
private slots:
	void about();
	void colorCrash();
private:
	void createStatusBar();
	void addActions();
	void addToolbar();
	void addMenu();

	QAction* mAction1;
	QAction* mAction2;
  QAction* mAction3;
  QAction* mAction4;
    QAction* mAboutQtAct;
    QAction* mAboutAct;

    QAction* mCrashAct;
};


}  // namespace ssc

#endif // SSCMAINWINDOW_H_
