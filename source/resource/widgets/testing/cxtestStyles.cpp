/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "catch.hpp"
#include "cxStyles.h"
#include "cxSettings.h"
#include <QColor>

namespace cxtest
{

namespace
{

struct StyleSettingGuard
{
	QString mOriginal;
	StyleSettingGuard() : mOriginal(cx::settings()->value("Gui/style").toString()) {}
	~StyleSettingGuard() { cx::settings()->setValue("Gui/style", mOriginal); }
	void set(const QString& style) { cx::settings()->setValue("Gui/style", style); }
};

} // namespace

TEST_CASE("Styles: useGrayStyle is false when style is not set", "[unit][resource][widgets]")
{
	StyleSettingGuard guard;
	guard.set("");
	CHECK_FALSE(cx::Styles::useGrayStyle());
}

TEST_CASE("Styles: useGrayStyle is true for 'gray'", "[unit][resource][widgets]")
{
	StyleSettingGuard guard;
	guard.set("gray");
	CHECK(cx::Styles::useGrayStyle());
}

TEST_CASE("Styles: useGrayStyle is true for 'grey'", "[unit][resource][widgets]")
{
	StyleSettingGuard guard;
	guard.set("grey");
	CHECK(cx::Styles::useGrayStyle());
}

TEST_CASE("Styles: useGrayStyle is false for unrecognized style", "[unit][resource][widgets]")
{
	StyleSettingGuard guard;
	guard.set("dark");
	CHECK_FALSE(cx::Styles::useGrayStyle());
}

TEST_CASE("Styles: getRed returns Qt::red in default style", "[unit][resource][widgets]")
{
	StyleSettingGuard guard;
	guard.set("");
	CHECK(cx::Styles::getRed() == QColor(Qt::red));
}

TEST_CASE("Styles: getRed returns custom red in gray style", "[unit][resource][widgets]")
{
	StyleSettingGuard guard;
	guard.set("gray");
	CHECK(cx::Styles::getRed() == QColor(165, 37, 52));
}

TEST_CASE("Styles: getGreen returns Qt::green in default style", "[unit][resource][widgets]")
{
	StyleSettingGuard guard;
	guard.set("");
	CHECK(cx::Styles::getGreen() == QColor(Qt::green));
}

TEST_CASE("Styles: getGreen returns custom green in gray style", "[unit][resource][widgets]")
{
	StyleSettingGuard guard;
	guard.set("gray");
	CHECK(cx::Styles::getGreen() == QColor(40, 165, 40));
}

TEST_CASE("Styles: getGray returns Qt::gray in default style", "[unit][resource][widgets]")
{
	StyleSettingGuard guard;
	guard.set("");
	CHECK(cx::Styles::getGray() == QColor(Qt::gray));
}

TEST_CASE("Styles: getGray returns custom gray in gray style", "[unit][resource][widgets]")
{
	StyleSettingGuard guard;
	guard.set("gray");
	CHECK(cx::Styles::getGray() == QColor(31, 35, 49));
}

TEST_CASE("Styles: getYellow returns Qt::yellow in default style", "[unit][resource][widgets]")
{
	StyleSettingGuard guard;
	guard.set("");
	CHECK(cx::Styles::getYellow() == QColor(Qt::yellow));
}

TEST_CASE("Styles: getYellow returns custom yellow in gray style", "[unit][resource][widgets]")
{
	StyleSettingGuard guard;
	guard.set("gray");
	CHECK(cx::Styles::getYellow() == QColor(227, 153, 11));
}

TEST_CASE("Styles: getGrayPalette sets expected button and window text colors", "[unit][resource][widgets]")
{
	QPalette palette = cx::Styles::getGrayPalette();
	CHECK(palette.color(QPalette::ButtonText) == QColor(240, 240, 240));
	CHECK(palette.color(QPalette::WindowText) == QColor(240, 240, 240));
}

TEST_CASE("Styles: getGrayPalette sets expected highlight and base colors", "[unit][resource][widgets]")
{
	QPalette palette = cx::Styles::getGrayPalette();
	CHECK(palette.color(QPalette::HighlightedText) == QColor(240, 240, 240));
	CHECK(palette.color(QPalette::Base) == QColor(12, 14, 22));
}

TEST_CASE("Styles: getGrayPalette sets expected light color for 3D effects", "[unit][resource][widgets]")
{
	QPalette palette = cx::Styles::getGrayPalette();
	CHECK(palette.color(QPalette::Light) == QColor(105, 110, 130));
}

} // namespace cxtest
