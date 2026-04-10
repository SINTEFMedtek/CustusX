#include "cxStyles.h"
#include <QApplication>
#include <QPainter>
#include <QProxyStyle>
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
static const QColor grayText(180, 180, 180);
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

namespace
{

class GrayStyle : public QProxyStyle
{
public:
	using QProxyStyle::QProxyStyle;

	QIcon standardIcon(StandardPixmap sp, const QStyleOption* option = nullptr, const QWidget* widget = nullptr) const override
	{
		QIcon icon;
		if (sp == QStyle::SP_TitleBarCloseButton || sp == QStyle::SP_DockWidgetCloseButton)
			icon = makeIcon([](QPainter& p) {
				p.setRenderHint(QPainter::Antialiasing);
				p.setPen(QPen(grayText, 1.5, Qt::SolidLine, Qt::RoundCap));
				p.drawLine(3, 3, 13, 13);
				p.drawLine(13, 3, 3, 13);
			});
		else if (sp == QStyle::SP_TitleBarNormalButton)
			icon = makeIcon([](QPainter& p) {
				p.setPen(QPen(grayText, 1, Qt::SolidLine, Qt::SquareCap));
				p.setBrush(Qt::NoBrush);
				p.drawRect(5, 2, 8, 8);  // back window
				p.fillRect(2, 5, 9, 9, darkGray1);  // cover back window overlap
				p.drawRect(2, 5, 8, 8);  // front window
			});
		else
			icon = QProxyStyle::standardIcon(sp, option, widget);
		return icon;
	}


private:
	template<typename DrawFunc>
	static QIcon makeIcon(DrawFunc draw)
	{
		QPixmap pixmap(16, 16);
		pixmap.fill(Qt::transparent);
		QPainter painter(&pixmap);
		draw(painter);
		return QIcon(pixmap);
	}
};

} // namespace

void Styles::setGrayStyle()
{
	// GrayStyle wraps the Windows style and overrides close button icons to be light-colored.
	qApp->setStyle(new GrayStyle(QStyleFactory::create("Windows")));
	qApp->setPalette(cx::Styles::getGrayPalette());
	// Palette alone does not control QDockWidget title bars and close/float buttons —
	// Qt's Windows style renders them via system drawing that ignores the palette.
	// A stylesheet is required to set their colors explicitly.
	qApp->setStyleSheet(
		"QDockWidget::title {"
		"  background: rgb(25, 65, 85);"   // blue title bar
		"  padding-left: 4px;"
		"}"
		"QDockWidget::close-button, QDockWidget::float-button {"
		"  border: 1px solid rgb(105, 110, 130);"
		"  border-radius: 2px;"
		"  background: rgb(31, 35, 49);"
		"}"
		"QDockWidget::close-button:hover {"
		"  background: rgb(165, 37, 52);"  // red on hover
		"}"
		"QDockWidget::float-button:hover {"
		"  background: rgb(49, 52, 65);"
		"}"
	);
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

	//3D effects: explicitly set derived colors that Qt auto-generates poorly for dark themes
	//Allows more contrast between selected/not selected structures in the view righ-click menu
	palette.setColor(QPalette::Light, lightDarkGray);
	//Dark and Shadow are used for the shadow edges of 3D buttons (e.g. close/float buttons).
	//Auto-generated from a dark button color they become near-black and invisible against the button.
	palette.setColor(QPalette::Dark, darkGray1);
	palette.setColor(QPalette::Shadow, QColor(0, 0, 0));
	//Mid is the button face tone between Button and Dark; make it slightly lighter than Button
	palette.setColor(QPalette::Mid, darkGray2);

	//Active title bars (e.g. dock widget titles) — gives them a distinct color from the background
	palette.setColor(QPalette::Highlight, blue);
	palette.setColor(QPalette::HighlightedText, whiteText);

	//combobox, group text. Probably the text where textBackgroundColor is used
	// palette.setColor(QPalette::Text, generalTextColor);

	return palette;

}

}//cx
