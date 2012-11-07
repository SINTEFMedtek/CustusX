#ifndef CXFILEPREVIEWWIDGET_H_
#define CXFILEPREVIEWWIDGET_H_

#include "cxBaseWidget.h"
#include "boost/shared_ptr.hpp"
class QTextDocument;
class QTextEdit;
class QLabel;
class QPushButton;
class QFileSystemWatcher;
class QFile;
#include <QSyntaxHighlighter>

namespace cx
{
/**
 *
 * \brief View a xml document.
 * \ingroup cxGUI
 *
 * A viewer widget for a xml file with syntax highlightning.
 *
 * \date Mar 22, 2011
 * \author Janne Beate Bakeng, SINTEF
 */

class FilePreviewWidget: public BaseWidget
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
	void previewFileSlot(const QString& absoluteFilePath);
	void saveSlot();
	void textChangedSlot();

private:
	void watchFile(bool on);

	QTextDocument* 						mTextDocument;
	QTextEdit* 								mTextEdit;
	QLabel*										mFileNameLable;
	QSyntaxHighlighter* 			mSyntaxHighlighter;

	QPushButton* 							mSaveButton;

	QFileSystemWatcher*				mFileSystemWatcher;
	boost::shared_ptr<QFile>	mCurrentFile;
};

}

#endif /* CXFILEPREVIEWWIDGET_H_ */
