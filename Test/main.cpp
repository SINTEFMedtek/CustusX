#include <string>

#include <QApplication>

#include "sscTestInterface.h"

/** Test app for SSC
 */
int main(int argc, char **argv)
{
	QApplication app(argc, argv);

	ssc::TestInterface testInterface;

	int val = app.exec();
	return val;
	//return 0;
}
