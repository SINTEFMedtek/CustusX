/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
=========================================================================*/

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
