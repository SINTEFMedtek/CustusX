/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXELASTIXSYNTAXHIGHLIGHTER_H_
#define CXELASTIXSYNTAXHIGHLIGHTER_H_

#include <QtWidgets>


namespace cx
{

/** Provides syntax highlighting for ElastiX parameter files.
 *
 * \ingroup org_custusx_registration_method_commandline
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
