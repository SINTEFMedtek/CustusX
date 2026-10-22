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
#include <QDockWidget>
#include <QMainWindow>
#include <QMenuBar>
#include <QStatusBar>
#include <QToolBar>
#include <QPushButton>
#include <QScrollArea>
#include <QSet>
#include <QTabWidget>

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
	QMap<QString, QUrl> links = engine->engine()->linksForKeyword(id);
	REQUIRE(links.size()==1);

	// Check the page content, not the URL: newer Doxygen versions escape underscores in the anchor.
	CHECK(QString(engine->engine()->fileData(links.first())).contains("User Documentation"));
}

TEST_CASE("org.custusx.help: HelpWidget displays initial help text", "[unit][plugins][org.custusx.help]")
{
	cxtest::TestHelpFixture fixture;
	fixture.setupHelpBrowserInsideMainWindow();

	// The overview is index.html; its anchor name depends on the Doxygen version.
	CHECK(fixture.browser->source().fileName() == "index.html");
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

	// CHECK("qthelp://org.custusx.core/doc/utility_widgets.html#console_widget" == fixture.browser->source().toString());
	CHECK(fixture.browser->toPlainText().contains("Console Widget"));

	fixture.shutdown();
}

namespace
{
cx::HelpEngine::HasKeywordFunction keywords(QStringList ids)
{
	QSet<QString> idSet;
	foreach (QString id, ids)
	{
		idSet.insert(id);
	}
	return [idSet](QString id) { return idSet.contains(id); };
}

QWidget* createWidget(QString name, QWidget* parent)
{
	QWidget* retval = new QWidget(parent);
	retval->setObjectName(name);
	return retval;
}

QDockWidget* createDockWidget(QWidget* content, QWidget* parent)
{
	QDockWidget* retval = new QDockWidget(parent);
	retval->setObjectName(content->objectName() + "DockWidget");
	QScrollArea* scroller = new QScrollArea;
	scroller->setWidget(content);
	retval->setWidget(scroller);
	return retval;
}
} // namespace

TEST_CASE("org.custusx.help: a focused or clicked widget gives the keyword of the nearest parent", "[unit][plugins][org.custusx.help]")
{
	QWidget mainWindow;
	QWidget* trackingTest = createWidget("tracking_test_widget", &mainWindow);
	QWidget* button = createWidget("", createWidget("group", trackingTest));
	cx::HelpEngine::HasKeywordFunction hasKeyword = keywords(QStringList() << "tracking_test_widget");

	CHECK(cx::HelpEngine::findKeywordUpwards(button, hasKeyword) == "tracking_test_widget");
	CHECK(cx::HelpEngine::findKeywordUpwards(trackingTest, hasKeyword) == "tracking_test_widget");
	CHECK(cx::HelpEngine::findKeywordUpwards(&mainWindow, hasKeyword) == "");
}

TEST_CASE("org.custusx.help: a dock widget gives the keyword of its content", "[unit][plugins][org.custusx.help]")
{
	QWidget mainWindow;
	QWidget* trackingTest = createWidget("tracking_test_widget", NULL);
	createWidget("export_widget", trackingTest)->hide();
	QDockWidget* dockWidget = createDockWidget(trackingTest, &mainWindow);
	cx::HelpEngine::HasKeywordFunction hasKeyword = keywords(QStringList() << "tracking_test_widget" << "export_widget");

	CHECK(cx::HelpEngine::findKeywordInside(dockWidget, hasKeyword) == "tracking_test_widget");
	CHECK(cx::HelpEngine::findKeywordForClick(dockWidget, hasKeyword) == "tracking_test_widget");
}

TEST_CASE("org.custusx.help: hidden widgets inside a dock widget are skipped", "[unit][plugins][org.custusx.help]")
{
	QWidget mainWindow;
	QWidget* content = createWidget("content", NULL);
	createWidget("hidden_widget", content)->hide();
	createWidget("shown_widget", content);
	QDockWidget* dockWidget = createDockWidget(content, &mainWindow);

	CHECK(cx::HelpEngine::findKeywordInside(dockWidget, keywords(QStringList() << "hidden_widget" << "shown_widget")) == "shown_widget");
}

TEST_CASE("org.custusx.help: a widget without a keyword inside gives the keyword of its parent", "[unit][plugins][org.custusx.help]")
{
	QWidget outer;
	outer.setObjectName("outer_widget");
	QWidget* plain = createWidget("", &outer);

	CHECK(cx::HelpEngine::findKeywordInside(plain, keywords(QStringList() << "outer_widget")) == "outer_widget");
}

TEST_CASE("org.custusx.help: a clicked tab gives the keyword of the current page", "[unit][plugins][org.custusx.help]")
{
	QWidget outer;
	outer.setObjectName("outer_widget");
	QTabWidget* tabWidget = new QTabWidget(&outer);
	tabWidget->addTab(createWidget("page_a", NULL), "a");
	tabWidget->addTab(createWidget("page_b", NULL), "b");
	tabWidget->setCurrentIndex(1);
	cx::HelpEngine::HasKeywordFunction hasKeyword = keywords(QStringList() << "outer_widget" << "page_a" << "page_b");

	CHECK(cx::HelpEngine::findKeywordForClick(tabWidget->tabBar(), hasKeyword) == "page_b");
	tabWidget->setCurrentIndex(0);
	CHECK(cx::HelpEngine::findKeywordForClick(tabWidget->tabBar(), hasKeyword) == "page_a");
}

TEST_CASE("org.custusx.help: the help widget itself gives no keyword", "[unit][plugins][org.custusx.help]")
{
	QWidget mainWindow;
	QWidget* helpWidget = createWidget("help_widget", NULL);
	QWidget* insideHelp = createWidget("console_widget", helpWidget);
	QDockWidget* dockWidget = createDockWidget(helpWidget, &mainWindow);
	cx::HelpEngine::HasKeywordFunction hasKeyword = keywords(QStringList() << "console_widget");

	CHECK(cx::HelpEngine::findKeywordUpwards(insideHelp, hasKeyword) == "");
	CHECK(cx::HelpEngine::findKeywordInside(dockWidget, hasKeyword) == "");
	CHECK(cx::HelpEngine::findKeywordForClick(insideHelp, hasKeyword) == "");
}

TEST_CASE("org.custusx.help: clicks on tool bars, menus and the status bar give no keyword", "[unit][plugins][org.custusx.help]")
{
	QMainWindow mainWindow;
	mainWindow.setObjectName("main_window");
	QToolBar* toolBar = mainWindow.addToolBar("Help");
	QWidget* toolButton = createWidget("", toolBar);
	QWidget* inMenuBar = createWidget("", mainWindow.menuBar());
	QWidget* inStatusBar = createWidget("", mainWindow.statusBar());
	QWidget* view = createWidget("", new QWidget(&mainWindow));
	cx::HelpEngine::HasKeywordFunction hasKeyword = keywords(QStringList() << "main_window");

	CHECK(cx::HelpEngine::findKeywordForClick(toolButton, hasKeyword) == "");
	CHECK(cx::HelpEngine::findKeywordForClick(inMenuBar, hasKeyword) == "");
	CHECK(cx::HelpEngine::findKeywordForClick(inStatusBar, hasKeyword) == "");
	CHECK(cx::HelpEngine::findKeywordForClick(view, hasKeyword) == "main_window");
}

TEST_CASE("org.custusx.help: a dock widget is found from its tab title", "[unit][plugins][org.custusx.help]")
{
	QMainWindow mainWindow;
	QDockWidget* trackingTest = new QDockWidget("Tracking Test", &mainWindow);
	QDockWidget* exportWidget = new QDockWidget("Export", &mainWindow);
	QDockWidget* hiddenExport = new QDockWidget("Export", &mainWindow);
	hiddenExport->hide();
	mainWindow.addDockWidget(Qt::LeftDockWidgetArea, trackingTest);
	mainWindow.tabifyDockWidget(trackingTest, exportWidget);

	CHECK(cx::HelpEngine::findDockWidget(&mainWindow, "Tracking Test") == trackingTest);
	CHECK(cx::HelpEngine::findDockWidget(&mainWindow, "Export") == exportWidget);
	CHECK(cx::HelpEngine::findDockWidget(&mainWindow, "Unknown") == NULL);
}

TEST_CASE("org.custusx.help: of two dock widgets with the same title, the one closest to the tab bar is found", "[unit][plugins][org.custusx.help]")
{
	QMainWindow mainWindow;
	QDockWidget* left = new QDockWidget("Console", &mainWindow);
	QDockWidget* right = new QDockWidget("Console", &mainWindow);
	left->setGeometry(0, 0, 100, 100);
	right->setGeometry(500, 0, 100, 100);

	CHECK(cx::HelpEngine::findDockWidget(&mainWindow, "Console", QPoint(40, 120)) == left);
	CHECK(cx::HelpEngine::findDockWidget(&mainWindow, "Console", QPoint(560, 120)) == right);
}
