#include "cxStyles.h"
#include <QApplication>
#include <QStyleFactory>

#include "cxSettings.h"

namespace cx
{
static const QColor darkGray1(12, 14, 22);
static const QColor darkGray2(31, 35, 49);
static const QColor darkGray3(49, 52, 65);
static const QColor lightDarkGray(105, 110, 130);
static const QColor red(165, 37, 52);
static const QColor blue(25, 65, 85);
static const QColor green(40, 165, 40);
static const QColor yellow(227, 153, 11);

static const QColor blueText(119, 161, 226);
static const QColor whiteText(240, 240, 240);
static const QColor redText(255, 90, 90);
static const QColor greenText(110, 255, 115);


void Styles::setStyle()
{
	if(useGrayStyle())
		setGrayStyle();
	else
		setDefaultStyle();
}

bool Styles::useGrayStyle()
{
	bool retval = false;
	QString style = settings()->value("Gui/style").toString();
	if((style == "gray") || (style == "grey"))
		retval = true;
	return retval;
}

void Styles::setGrayStyle()
{
	setApplicationWindowsStyle();
	qApp->setPalette(cx::Styles::getGrayPalette());
}

void Styles::setDefaultStyle()
{

}

QColor Styles::getRed()
{
	if(useGrayStyle())
		return red;
	else
		return Qt::red;
}
QColor Styles::getGreen()
{
	if(useGrayStyle())
		return green;
	else
		return Qt::green;
}
QColor Styles::getGray()
{
	if(useGrayStyle())
		return darkGray2;
	else
		return Qt::gray;
}
QColor Styles::getYellow()
{
	if(useGrayStyle())
		return yellow;
	else
		return Qt::yellow;
}

QColor Styles::getTextColor()
{
	return qApp->palette().color(QPalette::WindowText);
}

// For mac (default style) just changing palette won't change elements like:
// top line, bottom line, sliders, combobox background, and toolbar.
//Configurable styles are QCommonStyle and QProxyStyle, but using Windows style for now
void Styles::setApplicationWindowsStyle()
{
	qApp->setStyle(QStyleFactory::create("Windows"));
}

QPalette Styles::getGrayPalette()
{
	QColor buttonColor = darkGray2;
	QColor backGroundColor = darkGray3;
	QColor buttonTextColor = whiteText;
	QColor generalTextColor = whiteText;
	QColor highlightTextColor = blueText;
	QColor textBackgroundColor = darkGray1;//Background color for text widgtes, and combobox background

	QPalette palette(buttonColor, backGroundColor);
	palette.setColor(QPalette::ButtonText, buttonTextColor);
	palette.setColor(QPalette::WindowText, generalTextColor);
	palette.setColor(QPalette::HighlightedText, highlightTextColor);
	palette.setColor(QPalette::Base, textBackgroundColor);

	//3D effects
	//Allows more contrast between selected/not selected structures in the view righ-click menu
	palette.setColor(QPalette::Light, lightDarkGray);

	//combobox, group text. Probably the text where textBackgroundColor is used
	// palette.setColor(QPalette::Text, generalTextColor);

	//TODO: Change 2nd color on default "Windows buttons":
	//rgb(53, 54, 55)
	//Changing button or background don't change this

	return palette;

}

}//cx
