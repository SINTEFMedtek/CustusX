#ifdef SSC_USE_OpenCL

#include "catch.hpp"
#include "cxOpenCLPrinter.h"

#include <QApplication>
#include <QPushButton>
#include <QTimer>

#include <testing/cxTestRenderSpeed.h>

namespace cxtest
{

TEST_CASE("OpenCL and Gui: Init OpenCL and show a button", "[unit][OpenCL][gui][hide]")
{
	cx::OpenCLPrinter::printPlatformAndDeviceInfo();

    QPushButton button ("Hello world !");
    button.show();

    QTimer::singleShot(1000, qApp, SLOT(quit()) );

    CHECK(true);
    qApp->exec();

}

TEST_CASE("OpenCL and Gui: Show a button and init OpenCL", "[unit][OpenCL][gui][hide]")
{
    QPushButton button ("Hello world !");
    button.show();

    QTimer::singleShot(1000, qApp, SLOT(quit()) );

	cx::OpenCLPrinter::printPlatformAndDeviceInfo();

	CHECK(true);
    qApp->exec();
}

TEST_CASE("OpenCL and OpenGl: Show a OpenGL view and init OpenCL", "[unit][OpenCL][OpenGL][gui][hide][segmentation_fault]")
{
    TestRenderSpeed helper;
    helper.testSingleView();

	cx::OpenCLPrinter::printPlatformAndDeviceInfo();

	CHECK(true);
}

TEST_CASE("OpenCL and OpenGl: Init OpenCL and show a OpenGL view.", "[unit][OpenCL][OpenGL][gui][hide]")
{
	cx::OpenCLPrinter::printPlatformAndDeviceInfo();

    TestRenderSpeed helper;
    helper.testSingleView();

	CHECK(true);
}

}
#endif //SSC_USE_OpenCL
