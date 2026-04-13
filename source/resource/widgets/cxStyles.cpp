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


QIcon Styles::screenshotIcon(const QString& label)
{
	QPixmap pixmap(48, 48);
	pixmap.fill(Qt::transparent);
	QPainter p(&pixmap);
	p.setRenderHint(QPainter::Antialiasing);

	// Camera body
	p.setPen(QPen(QColor(40, 120, 110), 1.5));
	p.setBrush(QColor(93, 207, 190));
	p.drawRoundedRect(2, 14, 44, 30, 4, 4);

	// Viewfinder bump
	QPolygonF bump;
	bump << QPointF(16, 14) << QPointF(20, 8) << QPointF(28, 8) << QPointF(32, 14);
	p.setPen(Qt::NoPen);
	p.drawPolygon(bump);
	p.setPen(QPen(QColor(40, 120, 110), 1.5));
	p.drawLine(QPointF(16, 14), QPointF(20, 8));
	p.drawLine(QPointF(20, 8), QPointF(28, 8));
	p.drawLine(QPointF(28, 8), QPointF(32, 14));

	// Lens housing
	p.setBrush(QColor(26, 26, 46));
	p.drawEllipse(QPoint(22, 29), 10, 10);

	// Lens glass
	p.setBrush(QColor(46, 74, 112));
	p.drawEllipse(QPoint(22, 29), 7, 7);

	// Label in bottom-right of camera body
	p.setPen(QColor(80, 10, 20));
	p.setFont(QFont("Arial", 11, QFont::Bold));
	p.drawText(QRect(32, 30, 14, 14), Qt::AlignCenter, label);

	return QIcon(pixmap);
}

namespace
{

// --- Workflow icon geometry constants ---
const int    kIconSize       = 64;
const QPoint kCircleCenter   = QPoint(32, 27);
const int    kCircleRadius   = 22;
const double kCircleBorder   = 2.0;
const int    kLabelY         = 30;   // text rect top — sits in the lower half of the circle
const int    kLabelHeight    = 28;
const int    kLabelFontSize  = 22;

// --- Workflow icon color palette ---
struct WorkflowPalette
{
	QColor circleFill;
	QColor circleBorder;
	QColor forwardArrow;
	QColor label;   // used for text in state icons and action arrows in save/revert icons
};

WorkflowPalette workflowPalette(bool gray)
{
	WorkflowPalette pal;
	if (gray)
	{
		pal.circleFill   = QColor(blue);
		pal.circleBorder = QColor(75, 130, 165);
		pal.forwardArrow = QColor(200, 170, 60);
		pal.label        = QColor(220, 220, 220);
	}
	else
	{
		pal.circleFill   = QColor(255, 250, 150);
		pal.circleBorder = QColor(140, 150, 20);
		pal.forwardArrow = QColor(67, 121, 111);
		pal.label        = QColor(75, 25, 5);
	}
	return pal;
}

// Draws the shared base: circle background + right-pointing forward arrow.
void drawWorkflowBase(QPainter& p, const WorkflowPalette& pal)
{
	p.setPen(QPen(pal.circleBorder, kCircleBorder));
	p.setBrush(pal.circleFill);
	p.drawEllipse(kCircleCenter, kCircleRadius, kCircleRadius);

	QPolygonF arrow;
	arrow << QPointF(16, 24) << QPointF(34, 24) << QPointF(34, 18)
	      << QPointF(49, 27) << QPointF(34, 36) << QPointF(34, 30)
	      << QPointF(16, 30);
	p.setPen(Qt::NoPen);
	p.setBrush(pal.forwardArrow);
	p.drawPolygon(arrow);
}

} // namespace

QIcon Styles::workflowStateIcon(const QString& label)
{
	WorkflowPalette pal = workflowPalette(useGrayStyle());
	QPixmap pixmap(kIconSize, kIconSize);
	pixmap.fill(Qt::transparent);
	QPainter p(&pixmap);
	p.setRenderHint(QPainter::Antialiasing);
	drawWorkflowBase(p, pal);

	p.setPen(pal.label);
	p.setFont(QFont("Arial", kLabelFontSize, QFont::Bold));
	p.drawText(QRect(0, kLabelY, kIconSize, kLabelHeight), Qt::AlignCenter, label);

	return QIcon(pixmap);
}

QIcon Styles::workflowStateSaveIcon()
{
	WorkflowPalette pal = workflowPalette(useGrayStyle());
	QPixmap pixmap(kIconSize, kIconSize);
	pixmap.fill(Qt::transparent);
	QPainter p(&pixmap);
	p.setRenderHint(QPainter::Antialiasing);
	drawWorkflowBase(p, pal);

	// Downward arrow — save current desktop layout
	// Shaft: width 10, head: width 20, tip protrudes below circle
	QPolygonF down;
	down << QPointF(27, 33) << QPointF(37, 33) << QPointF(37, 43)
	     << QPointF(42, 43) << QPointF(32, 57) << QPointF(22, 43)
	     << QPointF(27, 43);
	p.setPen(Qt::NoPen);
	p.setBrush(pal.label);
	p.drawPolygon(down);

	return QIcon(pixmap);
}

QIcon Styles::workflowStateRevertIcon()
{
	WorkflowPalette pal = workflowPalette(useGrayStyle());
	QPixmap pixmap(kIconSize, kIconSize);
	pixmap.fill(Qt::transparent);
	QPainter p(&pixmap);
	p.setRenderHint(QPainter::Antialiasing);
	drawWorkflowBase(p, pal);

	// Down-then-left L-shaped arrow — revert to default desktop layout
	// Vertical starts at forward-arrow midpoint (y=27), horizontal sits low in the icon.
	// Tips protrude past the circle edge intentionally.
	QPolygonF revert;
	revert << QPointF(42, 27) << QPointF(48, 27) << QPointF(48, 53)
	       << QPointF(25, 53) << QPointF(25, 59) << QPointF(13, 50)
	       << QPointF(25, 41) << QPointF(25, 47) << QPointF(42, 47);
	p.setPen(Qt::NoPen);
	p.setBrush(pal.label);
	p.drawPolygon(revert);

	return QIcon(pixmap);
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
	QColor highlightTextColor = whiteText;
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

	//combobox, group text. Probably the text where textBackgroundColor is used
	// palette.setColor(QPalette::Text, generalTextColor);

	return palette;

}

}//cx
