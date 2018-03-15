/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "snwSyntaxHighlighter.h"
#include <iostream>

namespace snw
{

SyntaxHighlighter::SyntaxHighlighter(QTextDocument* parent) : QSyntaxHighlighter(parent)
{

}

void SyntaxHighlighter::highlightBlock(const QString &text)
{
	QTextCharFormat format;
	QString pattern;

  //xml element: <elem>
	format = QTextCharFormat();
	format.setForeground(QColor("darkviolet"));
  format.setFontWeight(QFont::Bold);
  pattern = "<[^>]*>";
	this->applyFormat(text, format, pattern);

  //xml comment: <!comment>
  format = QTextCharFormat();
  format.setForeground(QColor("green"));
  format.setFontItalic(true);
  pattern = "<![^>]*>";
  applyFormat(text, format, pattern);

  //quotations: "text"
  format = QTextCharFormat();
  format.setForeground(QColor("blue"));
  pattern = "\"[^\"]*\"";
  applyFormat(text, format, pattern);
}

void SyntaxHighlighter::highlightTimestamp(const QString &text)
{
	QTextCharFormat format;
	QString pattern;

	// timestamp format: [12:30:00 :000] with some parts optional
	QString stampPattern = "^\\[?[0-9]{2}:[0-9]{2}:[0-9]{2}( :[0-9]{3})?\\]?";
	// heading part: timestamp + function name from SW_LOG.
	QString headingPattern = stampPattern+"\\s\\w*\\s--\\s";
	headingPattern = headingPattern+"|------->.*"; // add the SW_LOG control lines.

	// set entire header to bold
	format.setFontWeight(QFont::Bold);
	format.setForeground(Qt::black);
	applyFormat(text, format, headingPattern);

	// set the timestamp part to magenta
	format.setFontWeight(QFont::Bold);
	format.setForeground(Qt::darkMagenta);
	applyFormat(text, format, stampPattern);
}

void SyntaxHighlighter::applyFormat(const QString &text, const QTextCharFormat& format, const QString pattern)
{
	QRegExp expression(pattern);
	int index = text.indexOf(expression);
	while (index >= 0)
	{
		int length = expression.matchedLength();
//		std::cout << "---[hit] " << index << " " << length << " - " << text.mid(index,length).toStdString() << std::endl;
		setFormat(index, length, format);
		index = text.indexOf(expression, index + length);
	}
}


} // namespace snw
