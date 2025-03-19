#include "cxStyles.h"
#include <QApplication>
#include <QStyleFactory>

namespace cx
{
QColor Styles::darkGray1(12, 14, 22);
QColor Styles::darkGray2(31, 35, 49);
QColor Styles::darkGray3(49, 52, 65);
QColor Styles::red(165, 37, 52);
QColor Styles::blue(25, 65, 85);

QColor Styles::blueText(119, 161, 226);
QColor Styles::whiteText(240, 240, 240);
QColor Styles::redText(255, 90, 90);
QColor Styles::greenText(110, 255, 115);


void Styles::setGrayStyle()
{
	setApplicationWindowsStyle();
	qApp->setPalette(cx::Styles::getGrayPalette());
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

	//combobox, group text. Probably the text where textBackgroundColor is used
	// palette.setColor(QPalette::Text, generalTextColor);

	//TODO: Change 2nd color on default "Windows buttons":
	//rgb(53, 54, 55)
	//Changing button or background don't change this

	return palette;

}

}//cx
