#ifndef CXSTYLES_H
#define CXSTYLES_H

#include "cxGuiExport.h"
#include <QPalette>

namespace cx
{

class cxGui_EXPORT Styles
{
public:
	static void setGrayStyle();

	static void setApplicationWindowsStyle();
	static QPalette getGrayPalette();

	static QColor darkGray1, darkGray2, darkGray3, red, blue;
	static QColor blueText, whiteText, redText, greenText;

};

}//cx

#endif // CXSTYLES_H
