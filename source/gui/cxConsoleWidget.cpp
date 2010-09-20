#include "cxConsoleWidget.h"

#include <iostream>
#include <QTextBrowser>
#include <QVBoxLayout>

namespace cx
{

ConsoleWidget::ConsoleWidget(QWidget* parent) :
    QWidget(parent)
{
  this->setObjectName("ConsoleWidget");
  this->setWindowTitle("Console");

  // Create text browser
  mTextBrowser = new QTextBrowser(this);

  // Set it read only
  mTextBrowser->setReadOnly(true);

  // Do not show frames
  mTextBrowser->setFrameStyle(true);
  mTextBrowser->setFrameStyle(QFrame::Plain);

  // Define the text formating
  this->createTextCharFormats();

  // Use layout
  QVBoxLayout* layout = new QVBoxLayout;
  layout->addWidget(mTextBrowser);
  this->setLayout(layout);

  connect(ssc::messageManager(), SIGNAL(emittedMessage(Message)), this, SLOT(printMessage(Message)));
}

ConsoleWidget::~ConsoleWidget()
{}

void ConsoleWidget::printMessage(Message message)
{
  this->format(message);

  mTextBrowser->append(message.getPrintableMessage());
}

void ConsoleWidget::createTextCharFormats()
{
  mInfoFormat.setForeground(Qt::black);
  mWarningFormat.setForeground(QColor(255,140,0)); //dark orange
  mErrorFormat.setForeground(Qt::red);
  mDebugFormat.setForeground(Qt::blue);
}

void ConsoleWidget::format(Message message)
{
  switch(message.getMessageLevel())
  {
  case ssc::mlINFO:
    mTextBrowser->setCurrentCharFormat(mInfoFormat);
    break;
  case ssc::mlWARNING:
    mTextBrowser->setCurrentCharFormat(mWarningFormat);
    break;
  case ssc::mlERROR:
    mTextBrowser->setCurrentCharFormat(mErrorFormat);
    break;
  case ssc::mlDEBUG:
    mTextBrowser->setCurrentCharFormat(mDebugFormat);
    break;
  default:
    break;
  }
}

}//namespace cx
