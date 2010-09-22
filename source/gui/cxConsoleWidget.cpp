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
  mFormat[ssc::mlINFO].setForeground(Qt::black);
  mFormat[ssc::mlWARNING].setForeground(QColor(255,140,0));
  mFormat[ssc::mlERROR].setForeground(Qt::red);
  mFormat[ssc::mlDEBUG].setForeground(QColor(135,206,250));
  mFormat[ssc::mlCERR].setForeground(Qt::red);
  mFormat[ssc::mlCOUT].setForeground(QColor(34,139,34));

//
//  mInfoFormat.setForeground(Qt::black);
//  mWarningFormat.setForeground(QColor(255,140,0)); //dark orange
//  mErrorFormat.setForeground(Qt::red);
//  mDebugFormat.setForeground(QColor(135,206,250)); //sky blue
}

void ConsoleWidget::format(Message& message)
{
  if (!mFormat.count(message.getMessageLevel()))
    return;
  mTextBrowser->setCurrentCharFormat(mFormat[message.getMessageLevel()]);
//
//  switch(message.getMessageLevel())
//  {
//  case ssc::mlINFO:
//    mTextBrowser->setCurrentCharFormat(mInfoFormat);
//    break;
//  case ssc::mlWARNING:
//    mTextBrowser->setCurrentCharFormat(mWarningFormat);
//    break;
//  case ssc::mlERROR:
//    mTextBrowser->setCurrentCharFormat(mErrorFormat);
//    break;
//  case ssc::mlDEBUG:
//    mTextBrowser->setCurrentCharFormat(mDebugFormat);
//    break;
//  default:
//    break;
//  }
}

}//namespace cx
