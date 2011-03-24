#include <cxFilePreviewWidget.h>

#include <QTextEdit>
#include <QTextDocument>
#include <QTextStream>
#include "sscMessageManager.h"
#include "snwSyntaxHighlighter.h"

namespace cx
{

FilePreviewWidget::FilePreviewWidget(QWidget* parent) :
    WhatsThisWidget(parent),
    mTextDocument(new QTextDocument()),
    mTextEdit(new QTextEdit())
{
  QVBoxLayout* layout = new QVBoxLayout(this);
  layout->addWidget(mTextEdit);

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

void FilePreviewWidget::previewFileSlot(QString absoluteFilePath)
{
  if(mCurrentFile && mCurrentFile->isOpen())
    mCurrentFile->close();

  mCurrentFile = new QFile(absoluteFilePath);

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

}//namespace cx
