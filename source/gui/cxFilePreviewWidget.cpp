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
  mTextDocument->setPlainText("<tag> Test </tag>");
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
  QFile file(absoluteFilePath);
  if(!file.exists())
  {
    ssc::messageManager()->sendDebug("File "+absoluteFilePath+" does not exist.");
    return;
  }

  if(!file.open(QIODevice::ReadOnly))
  {
    ssc::messageManager()->sendWarning("Could not open file "+absoluteFilePath);
  }
  QTextStream stream(&file);
  QString text = stream.readAll();

  mTextDocument->setPlainText(text);

  //TODO close file???
}

}//namespace cx
