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

#ifndef CXELASTIXSYNTAXHIGHLIGHTER_H_
#define CXELASTIXSYNTAXHIGHLIGHTER_H_

#include <QtGui>

namespace cx
{

/**\brief Provides syntax highlighting for ElastiX parameter files.
 * \ingroup cx_module_registration
 *
 *  \date Feb 14, 2012
 *  \author christiana
 */
class ElastixSyntaxHighlighter : public QSyntaxHighlighter
{
public:
	ElastixSyntaxHighlighter(QTextDocument* parent);
	virtual void highlightBlock(const QString &text);
	virtual ~ElastixSyntaxHighlighter() {}
private:
	void highlightTimestamp(const QString &text);
	void applyFormat(const QString &text, const QTextCharFormat& format, const QString pattern);
};

} /* namespace cx */

#endif /* CXELASTIXSYNTAXHIGHLIGHTER_H_ */
