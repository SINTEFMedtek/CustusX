/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
typedef boost::shared_ptr<class FilePreviewProperty> FilePreviewPropertyPtr;

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
	FilePreviewWidget(QWidget* parent, FilePreviewPropertyPtr dataInterface = FilePreviewPropertyPtr(), QGridLayout* gridLayout = 0, int row = 0);
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

protected slots:
	void fileinterfaceChanged();

private:
	//void watchFile(bool on);
	void createAndAddEditorLayout(QGridLayout *gridLayout, int row);

	FilePreviewPropertyPtr mData;

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
