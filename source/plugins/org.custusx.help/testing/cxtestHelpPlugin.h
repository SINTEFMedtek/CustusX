#ifndef CXTESTHELPPLUGIN_H
#define CXTESTHELPPLUGIN_H

namespace cx
{
class MainWindow;
class HelpWidget;
}
class QTextBrowser;

namespace cxtest
{

class TestHelpFixture
{
public:
	TestHelpFixture();
	void setupHelpBrowserInsideMainWindow();
	void runApp();
	void printBrowserContents();
	void shutdown();

	cx::MainWindow* mw;
	cx::HelpWidget* helpWidget;
	QTextBrowser* browser;
};

}

#endif // CXTESTHELPPLUGIN_H
