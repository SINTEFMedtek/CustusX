#ifndef CXFILEPREVIEWWIDGET_H_
#define CXFILEPREVIEWWIDGET_H_

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

class FilePreviewWidget: public FileWatcherWidget
{
	Q_OBJECT

public:
	FilePreviewWidget(QWidget* parent);
	virtual ~FilePreviewWidget();

	virtual QString defaultWhatsThis() const;

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
