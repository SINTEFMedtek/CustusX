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

#ifndef CXFILEPREVIEWWIDGET_H_
#define CXFILEPREVIEWWIDGET_H_

#include "cxResourceWidgetsExport.h"

#include "cxFileWatcherWidget.h"
class QTextDocument;
class QTextEdit;
class QPushButton;
#include <QSyntaxHighlighter>

namespace cx
{
/**
 *
 * \brief View a xml document.
 *
 * A viewcx_guidget for a xml file with syntax highlightning.
 *
 * \ingroup cx_resource_widgets
 * \date Mar 22, 2011
 * \author Janne Beate Bakeng, SINTEF
 */

class cxResourceWidgets_EXPORT FilePreviewWidget: public FileWatcherWidget
{
	Q_OBJECT

public:
	FilePreviewWidget(QWidget* parent);
	virtual ~FilePreviewWidget();

	template<class SYNTAXHIGHLIGHTER>
	void setSyntaxHighLighter()
	{
		delete mSyntaxHighlighter;
		mSyntaxHighlighter = new SYNTAXHIGHLIGHTER(mTextDocument);
	}

public slots:
	virtual void previewFileSlot(const QString& absoluteFilePath);
	void saveSlot();
	void textChangedSlot();

private:
	//void watchFile(bool on);

	QTextDocument* 						mTextDocument;
	QTextEdit* 								mTextEdit;
	//QLabel*										mFileNameLabel;
	QSyntaxHighlighter* 			mSyntaxHighlighter;

	QPushButton* 							mSaveButton;

	//QFileSystemWatcher*				mFileSystemWatcher;
	//boost::shared_ptr<QFile>	mCurrentFile;
};

}

#endif /* CXFILEPREVIEWWIDGET_H_ */
