#ifndef CXSTYLES_H
#define CXSTYLES_H

#include "cxResourceWidgetsExport.h"
#include <QPalette>

namespace cx
{

class cxResourceWidgets_EXPORT Styles
{
public:
	static void setStyle();
	static void setGrayStyle();
	static void setDefaultStyle();
	static bool useGrayStyle();

	static QColor getRed();
	static QColor getGreen();
	static QColor getGray();
	static QColor getYellow();
	static QColor getTextColor();

	static void setApplicationWindowsStyle();
	static QPalette getGrayPalette();
};

}//cx

#endif // CXSTYLES_H
