/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef SNWSYNTAXHIGHLIGHTER_H_
#define SNWSYNTAXHIGHLIGHTER_H_

#include "cxResourceWidgetsExport.h"

#include <QSyntaxHighlighter>

namespace snw
{

/**\brief Provides syntax highlighting for sonowand log files.
 *
 * Adapted for CustusX use: Simple xml highlighter.
 *
 * \ingroup cx_resource_widgets
 */
class cxResourceWidgets_EXPORT SyntaxHighlighter : public QSyntaxHighlighter
{
public:
	SyntaxHighlighter(QTextDocument* parent);
	virtual void highlightBlock(const QString &text);
	virtual ~SyntaxHighlighter() {}
private:
	void highlightTimestamp(const QString &text);
	void applyFormat(const QString &text, const QTextCharFormat& format, const QString pattern);
};

}

#endif /*SNWSYNTAXHIGHLIGHTER_H_*/
