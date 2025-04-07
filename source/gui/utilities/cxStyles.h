#ifndef CXSTYLES_H
#define CXSTYLES_H

#include "cxGuiExport.h"
#include <QPalette>

namespace cx
{

class cxGui_EXPORT Styles
{
public:
	static void setStyle();
	static void setGrayStyle();
	static void setDefaultStyle();

	static QColor getRed();
	static QColor getGreen();
	static QColor getGray();
	static QColor getYellow();

	static void setApplicationWindowsStyle();
	static QPalette getGrayPalette();

	static QColor darkGray1, darkGray2, darkGray3, lightDarkGray, red, blue, green, yellow;
	static QColor blueText, whiteText, redText, greenText;

};

}//cx

#endif // CXSTYLES_H
