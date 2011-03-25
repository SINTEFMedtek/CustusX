#include <cxFilePreviewWidget.h>

#include <QTextEdit>
#include <QTextDocument>
#include <QPushButton>
#include <QTextStream>
#include <QFileInfo>
#include <QFileSystemWatcher>
#include "sscMessageManager.h"
#include "snwSyntaxHighlighter.h"

namespace cx
{

FilePreviewWidget::FilePreviewWidget(QWidget* parent) :
    WhatsThisWidget(parent),
    mTextDocument(new QTextDocument()),
    mTextEdit(new QTextEdit()),
    mEditButton(new QPushButton("Edit")),
    mFileSystemWatcher(new QFileSystemWatcher())
{
  connect(mEditButton, SIGNAL(clicked()), this, SLOT(editSlot()));
  connect(mFileSystemWatcher, SIGNAL(fileChanged(const QString&)), this, SLOT(previewFileSlot(const QString&)));

  QHBoxLayout* buttonLayout = new QHBoxLayout();
  buttonLayout->addStretch();
  buttonLayout->addWidget(mEditButton);

  QVBoxLayout* layout = new QVBoxLayout(this);
  layout->addWidget(mTextEdit);
  layout->addLayout(buttonLayout);

  mTextEdit->setDocument(mTextDocument);
  mTextEdit->setReadOnly(true);
  mTextEdit->setLineWrapMode(QTextEdit::NoWrap);

  new snw::SyntaxHighlighter(mTextDocument);
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

void FilePreviewWidget::previewFileSlot(const QString& absoluteFilePath)
{
  if(mCurrentFile)
  {
    if(mCurrentFile->isOpen())
    {
      this->watchFile(false);
      mCurrentFile->close();
    }
  }

  mCurrentFile = new QFile(absoluteFilePath);
  this->watchFile(true);

  if(!mCurrentFile->exists())
  {
    ssc::messageManager()->sendDebug("File "+absoluteFilePath+" does not exist.");
    return;
  }

  if(!mCurrentFile->open(QIODevice::ReadOnly))
  {
    ssc::messageManager()->sendWarning("Could not open file "+absoluteFilePath);
  }
  QTextStream stream(mCurrentFile);
  QString text = stream.readAll();

  mTextDocument->setPlainText(text);
}

void FilePreviewWidget::editSlot()
{
  if(!mCurrentFile)
    return;

  QFileInfo info(*mCurrentFile);
  QUrl url("file:/"+info.absoluteFilePath());
  QDesktopServices::openUrl(url);
}

void FilePreviewWidget::watchFile(bool on)
{
  if(!mCurrentFile)
    return;

  QFileInfo info(*mCurrentFile);

  if(on)
    mFileSystemWatcher->addPath(info.absoluteFilePath());
  else
    mFileSystemWatcher->removePath(info.absoluteFilePath());
}

}//namespace cx
