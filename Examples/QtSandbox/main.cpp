#include <string>
#include <QApplication>
#include "sscMainWindow.h"

/** Test app for SSC
 */
int main(int argc, char **argv)
{
    //Q_INIT_RESOURCE(resource); // seems to be uneccesary... need if rc in a lib.

	QApplication app(argc, argv);
	app.setAttribute(Qt::AA_DontShowIconsInMenus, false);

	ssc::MainWindow mainWindow;
	mainWindow.show();

	int val = app.exec();
	return val;
}
