#include <cxFilePreviewWidget.h>

#include <QTextEdit>
#include <QLabel>
#include <QTextDocument>
#include <QPushButton>
#include <QTextStream>
//#include <QFileInfo>
#include <iostream>
#include "sscTypeConversions.h"
#include <QFileSystemWatcher>
#include "sscMessageManager.h"
#include "snwSyntaxHighlighter.h"

namespace cx
{

FilePreviewWidget::FilePreviewWidget(QWidget* parent) :
		FileWatcherWidget(parent, "FilePreviewWidget", "File Preview"),
	mTextDocument(new QTextDocument(this)),
	mTextEdit(new QTextEdit(this)),
    //mFileNameLabel(new QLabel()),
	mSaveButton(new QPushButton("Save", this))
    //mFileSystemWatcher(new QFileSystemWatcher())
{
	mSyntaxHighlighter = NULL;
  connect(mSaveButton, SIGNAL(clicked()), this, SLOT(saveSlot()));
  mSaveButton->setEnabled(false);
  //connect(mFileSystemWatcher, SIGNAL(fileChanged(const QString&)), this, SLOT(previewFileSlot(const QString&)));

  QHBoxLayout* buttonLayout = new QHBoxLayout();
  buttonLayout->addStretch();
  buttonLayout->addWidget(mSaveButton);

  QVBoxLayout* layout = new QVBoxLayout(this);
  layout->setMargin(0);
  layout->addWidget(mTextEdit);
  //layout->addWidget(mFileNameLabel);
  layout->addLayout(buttonLayout);

  connect(mTextEdit, SIGNAL(textChanged()), this, SLOT(textChangedSlot()));
  mTextEdit->setDocument(mTextDocument);
//  mTextEdit->setReadOnly(true);
  mTextEdit->setLineWrapMode(QTextEdit::NoWrap);

  this->setSyntaxHighLighter<snw::SyntaxHighlighter>();
}

FilePreviewWidget::~FilePreviewWidget()
{}

QString FilePreviewWidget::defaultWhatsThis() const
{
  return "<html>"
      "<h3>File preview.</h3>"
      "<p>Lets you preview and edit a file.</p>"
      "<p><i>Click save to keep your changes.</i></p>"
      "</html>";
}

void FilePreviewWidget::textChangedSlot()
{
	mSaveButton->setEnabled(mTextDocument->isModified());
}

void FilePreviewWidget::previewFileSlot(const QString& absoluteFilePath)
{
  mSaveButton->setEnabled(false);
//  if(mCurrentFile)
//  {
//    if(mCurrentFile->isOpen())
//    {
//      this->watchFile(false);
//      mCurrentFile->close();
//    }
//  }
//
//  mCurrentFile.reset(new QFile(absoluteFilePath));
//  this->watchFile(true);
//
//  if(!mCurrentFile->exists())
//  {
////    ssc::messageManager()->sendDebug("File "+absoluteFilePath+" does not exist.");
//    return;
//  }

//  if(!mCurrentFile->open(QIODevice::ReadOnly))
//  {
//    ssc::messageManager()->sendWarning("Could not open file "+absoluteFilePath);
//  }
  if(!this->internalOpenNewFile(absoluteFilePath))
  	return;

  QTextStream stream(mCurrentFile.get());
  QString text = stream.readAll();
  mCurrentFile->close();

  mTextDocument->setPlainText(text);
  mTextDocument->setModified(false);
//  mSaveButton->setEnabled(true);

  //QString elideText = fontMetrics().elidedText(absoluteFilePath, Qt::ElideLeft, mTextEdit->width());
  //mFileNameLabel->setText(elideText);

  this->textChangedSlot();
}

void FilePreviewWidget::saveSlot()
{
  if(!mCurrentFile)
    return;

  if(!mCurrentFile->open(QIODevice::WriteOnly | QIODevice::Truncate))
  {
    ssc::messageManager()->sendWarning("Could not open file "+mCurrentFile->fileName());
    return;
  }

  mCurrentFile->write(mTextDocument->toPlainText().toLatin1());
  mCurrentFile->close();

//  QFileInfo info(*mCurrentFile);
//  QUrl url("file:/"+info.absoluteFilePath());
//  QDesktopServices::openUrl(url);

  mTextDocument->setModified(false);

}

//void FilePreviewWidget::watchFile(bool on)
//{
//	if (!mCurrentFile)
//		return;
//
//	QFileInfo info(*mCurrentFile);
//
//	if (on)
//	{
//		if (!mFileSystemWatcher->files().contains(info.absoluteFilePath()))
//			mFileSystemWatcher->addPath(info.absoluteFilePath());
//	}
//	else
//	{
//		mFileSystemWatcher->removePath(info.absoluteFilePath());
//	}
//}

}//namespace cx
