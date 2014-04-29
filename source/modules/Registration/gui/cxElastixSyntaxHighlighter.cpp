// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.

#include "cxElastixSyntaxHighlighter.h"
#include <iostream>

namespace cx
{

ElastixSyntaxHighlighter::ElastixSyntaxHighlighter(QTextDocument* parent) :
				QSyntaxHighlighter(parent)
{

}

void ElastixSyntaxHighlighter::highlightBlock(const QString &text)
{
	QTextCharFormat format;
	QString pattern;

	// ElastiX parameters: (name value)
	format = QTextCharFormat();
	format.setForeground(QColor("black"));
	format.setFontWeight(75);
	pattern = "\\([^\\)]*\\)";
	applyFormat(text, format, pattern);

	//quotations: "text"
	format = QTextCharFormat();
	format.setForeground(QColor("blue"));
	pattern = "\"[^\"]*\"";
	applyFormat(text, format, pattern);

	//numbers: float or integer
	format = QTextCharFormat();
	format.setForeground(QColor("red"));
	pattern = "\\d[\\d|\\.]*";
	applyFormat(text, format, pattern);

	// ElastiX comment: // comment line
	format = QTextCharFormat();
	format.setForeground(QColor(63, 127, 95));
	pattern = "//.*";
//  pattern = "//[^\\n]*\\n";
	applyFormat(text, format, pattern);
}

void ElastixSyntaxHighlighter::highlightTimestamp(const QString &text)
{
	QTextCharFormat format;
	QString pattern;

	// timestamp format: [12:30:00 :000] with some parts optional
	QString stampPattern = "^\\[?[0-9]{2}:[0-9]{2}:[0-9]{2}( :[0-9]{3})?\\]?";
	// heading part: timestamp + function name from SW_LOG.
	QString headingPattern = stampPattern + "\\s\\w*\\s--\\s";
	headingPattern = headingPattern + "|------->.*"; // add the SW_LOG control lines.

	// set entire header to bold
	format.setFontWeight(QFont::Bold);
	format.setForeground(Qt::black);
	applyFormat(text, format, headingPattern);

	// set the timestamp part to magenta
	format.setFontWeight(QFont::Bold);
	format.setForeground(Qt::darkMagenta);
	applyFormat(text, format, stampPattern);
}

void ElastixSyntaxHighlighter::applyFormat(const QString &text, const QTextCharFormat& format, const QString pattern)
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

} /* namespace cx */
