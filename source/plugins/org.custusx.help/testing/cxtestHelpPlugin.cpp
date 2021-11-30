/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#include "catch.hpp"
#include "cxtestHelpPlugin.h"
#include "cxLogicManager.h"
#include "cxDataLocations.h"
#include "cxMainWindow.h"
#include "cxMainWindowApplicationComponent.h"
#include "cxHelpWidget.h"
#include "cxHelpEngine.h"
#include <QTextBrowser>
#include "cxConsoleWidget.h"
#include <QHelpEngine>
#include <QTimer>
#include "cxUtilHelpers.h"

namespace cxtest
{

TestHelpFixture::TestHelpFixture() : mw(NULL), helpWidget(NULL), browser(NULL)
{

}

void TestHelpFixture::setupHelpBrowserInsideMainWindow()
{
	cx::DataLocations::setTestMode();

	cx::ApplicationComponentPtr mainwindow(new cx::MainWindowApplicationComponent<cx::MainWindow>());
	cx::LogicManager::initialize(mainwindow);

	mw = dynamic_cast<cx::MainWindowApplicationComponent<cx::MainWindow>*>(mainwindow.get())->mMainWindow;
	REQUIRE(mw!=NULL);

	helpWidget = mw->findChild<cx::HelpWidget*>("help_widget");
	REQUIRE(helpWidget!=NULL);
	helpWidget->forcePrePaint(); // explicitly populate helpWidget

	browser = helpWidget->findChild<QTextBrowser*>();
	REQUIRE(browser!=NULL);
}

void TestHelpFixture::runApp(int milliseconds)
{
	QTimer::singleShot(milliseconds, qApp, SLOT(quit()));
	qApp->exec();
}

void TestHelpFixture::printBrowserContents()
{
	std::cout << "====================================" << std::endl;
	std::cout << "SOURCE: " <<  browser->source().toString().toStdString() << std::endl;
	std::cout << "PLAIN CONTENT: " <<  browser->toPlainText().toStdString() << std::endl;
	std::cout << "====================================" << std::endl;
}

void TestHelpFixture::shutdown()
{
	cx::LogicManager::shutdown();
}

} // namespace cxtest

// --------------------------------------------------------
// --------------------------------------------------------
// --------------------------------------------------------

TEST_CASE("org.custusx.help: HelpEngine loads a page", "[unit][plugins][org.custusx.help]")
{
	cx::DataLocations::setTestMode();

	cx::HelpEnginePtr engine(new cx::HelpEngine);
	REQUIRE(engine->engine());

	QString id = "user_doc_overview";
	QMap<QString, QUrl> links = engine->engine()->linksForIdentifier(id);
	REQUIRE(links.size()==1);

	CHECK(QString(links.first().toString()).contains(id));
}

TEST_CASE("org.custusx.help: HelpWidget displays initial help text", "[unit][plugins][org.custusx.help]")
{
	cxtest::TestHelpFixture fixture;
	fixture.setupHelpBrowserInsideMainWindow();

	CHECK(fixture.browser->source().toString().contains("user_doc_overview"));
	// this can be a tricky test for overriding applications: they must have this string in the main page in order to
	// succeed.
	CHECK(fixture.browser->toPlainText().contains("CustusX"));
	CHECK(fixture.browser->toPlainText().contains("User Documentation"));

//	fixture.printBrowserContents();

	fixture.shutdown();
}

TEST_CASE("org.custusx.help: HelpWidget displays Console Widget help on focus", "[integration][plugins][org.custusx.help][unstable]")
{
	for (int i=0; i<50; ++i)
	{
		cx::sleep_ms(20);
		qApp->processEvents();
	}
	cxtest::TestHelpFixture fixture;
	fixture.setupHelpBrowserInsideMainWindow();

	cx::ConsoleWidget* consoleWidget = fixture.mw->findChild<cx::ConsoleWidget*>();
	REQUIRE(consoleWidget!=NULL);

	// this test is a bit fragile:
	//  - the help system sets an initial page aftr 100ms.
	//  - this test focuses after 400ms
	//  - qapp runs for 600ms
	QTimer::singleShot(400, consoleWidget, SLOT(setFocus()));

	fixture.runApp(600);

//	fixture.printBrowserContents();

	CHECK("qthelp://org.custusx.core/doc/utility_widgets.html#console_widget" == fixture.browser->source().toString());
	CHECK(fixture.browser->toPlainText().contains("Console Widget"));

	fixture.shutdown();
}
