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

	helpWidget = mw->findChild<cx::HelpWidget*>("HelpWidget");
	REQUIRE(helpWidget!=NULL);
	helpWidget->forcePrePaint(); // explicitly populate helpWidget

	browser = helpWidget->findChild<QTextBrowser*>();
	REQUIRE(browser!=NULL);
}

void TestHelpFixture::runApp()
{
	QTimer::singleShot(300, qApp, SLOT(quit()));
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

	QString id = "mainpage_overview";
	QMap<QString, QUrl> links = engine->engine()->linksForIdentifier(id);
	REQUIRE(links.size()==1);

	CHECK("qthelp://org.custusx.core/doc/index.html#mainpage_overview" == links.first().toString());
//	std::cout << "LINKS: " << links.first().toString().toStdString() << std::endl;
}


TEST_CASE("org.custusx.help: HelpWidget displays initial help text", "[unit][plugins][org.custusx.help]")
{
	cxtest::TestHelpFixture fixture;
	fixture.setupHelpBrowserInsideMainWindow();

	CHECK("qthelp://org.custusx.core/doc/index.html#mainpage_overview" == fixture.browser->source().toString());
	CHECK(fixture.browser->toPlainText().contains("CustusX User Documentation"));

//	fixture.printBrowserContents();

	fixture.shutdown();
}

TEST_CASE("org.custusx.help: HelpWidget displays Console Widget help on focus", "[integration][plugins][org.custusx.help]")
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
	QMetaObject::invokeMethod(consoleWidget, "setFocus", Qt::QueuedConnection);

	fixture.runApp();

//	fixture.printBrowserContents();

	CHECK("qthelp://org.custusx.core/doc/utility_widgets.html#console_widget" == fixture.browser->source().toString());
	CHECK(fixture.browser->toPlainText().contains("Console Widget"));

	fixture.shutdown();
}
