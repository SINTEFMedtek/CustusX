/**
 * cxMain.cpp
 *
 * \brief
 *
 * \date Jan 20, 2009
 * \\author jbake
 */

#include <QApplication>
#include <iostream>
#include "cxMainWindow.h"
#include "sscMessageManager.h"

#include "cxUsReconstructionPlugin.h"
#include "cxAcquisitionPlugin.h"
#include "cxCalibrationPlugin.h"
#include "cxAlgorithmPlugin.h"
#include "cxRegistrationPlugin.h"

#include "sscTypeConversions.h"
void test()
{
	std::cout << "==============" << std::endl;
	QString text(""
	"(Transform \"EulerTransform\")\n"
	"(NumberOfParameters 6)\n"
	"(TransformParameters -0.000094 0.000040 0.000042 10.007466 4.993696 -1.956600)\n"
	"(InitialTransformParametersFileName \"NoInitialTransform\")\n"
	"(HowToCombineTransforms \"Compose\")");
	std::cout << text << std::endl << std::endl;
	QStringList list;

	QString regexpStr = QString(""
//		"\b"
		"\\s*" // match zero or more whitespace
		"\\("  // match beginning (
		"\\s*" // match zero or more whitespace
		"%1"   // key
		"\\s+" // match one or more whitespace
		"("    // start grab value
		"[^\\)]*"
		")"    // end grab value
		"\\)"  // match ending )
		).arg("Transform");
//	QString regexp = QString(""
//		"\\("
//		"\\s*"
//		"%1"
//		"[^\\)]*"
//		"\\)"
//		"").arg("Transform");
	QStringList lines = text.split('\n');
	QRegExp rx(regexpStr);

	if (lines.indexOf(rx)>=0)
	{
		std::cout << "FOUND0 " << rx.cap(0) << std::endl;
		std::cout << "FOUND1 " << rx.cap(1) << std::endl;
	}

	std::cout << regexpStr << std::endl;
	int pos=0;
	 while ((pos = rx.indexIn(text, pos)) != -1)
	 {
		 std::cout << "hit! " << pos << " : " << rx.cap(0) << std::endl;
	     list << rx.cap(1);
	     pos += rx.matchedLength();
	 }

	 std::cout << "results:\n" << list.join("\n\n") << std::endl;
//
//	std::cout << regexp << std::endl;
//	int hit = text.indexOf(regexp);
//
//	if (hit<0)
//		return;
//
////	QString line = lines[hit];
////	line = line.trimmed();
//	std::cout << hit << " -- "  << std::endl;

	std::cout << "==============" << std::endl;
}

int main(int argc, char *argv[])
{
//	test();
//	return 0;

  Q_INIT_RESOURCE(cxResources);
  
  QApplication app(argc, argv);
  app.setOrganizationName("SINTEF");
  app.setOrganizationDomain("www.sintef.no");
  app.setApplicationName("CustusX3");
  app.setWindowIcon(QIcon(":/icons/CustusX.png"));
//  app.setWindowIcon(QIcon(":/icons/.png"));
  app.setAttribute(Qt::AA_DontShowIconsInMenus, false);

  cx::MainWindow::initialize();

	std::vector<cx::PluginBasePtr> plugins;

	cx::UsReconstructionPluginPtr reconstructionPlugin(new cx::UsReconstructionPlugin());
	plugins.push_back(reconstructionPlugin);
	cx::AcquisitionPluginPtr acquisitionPlugin(new cx::AcquisitionPlugin(reconstructionPlugin->getReconstructer()));
	plugins.push_back(   acquisitionPlugin);
	cx::CalibrationPluginPtr calibrationPlugin(new cx::CalibrationPlugin(acquisitionPlugin->getAcquisitionData()));
	plugins.push_back(   calibrationPlugin);
	cx::AlgorithmPluginPtr algorithmPlugin(new cx::AlgorithmPlugin());
	plugins.push_back(   algorithmPlugin);
	cx::RegistrationPluginPtr registrationPlugin(new cx::RegistrationPlugin(acquisitionPlugin->getAcquisitionData()));
	plugins.push_back(   registrationPlugin);


	cx::MainWindow* mainWin = new cx::MainWindow(plugins);


  //mainWin->show();
#ifdef __APPLE__ // needed on mac for bringing to front: does the opposite on linux
  mainWin->activateWindow();
#endif
  mainWin->raise();

  int retVal = app.exec();

  plugins.clear();
  delete mainWin;
  cx::MainWindow::shutdown(); // shutdown all global resources, _after_ gui is deleted.
  return retVal;
  
}
