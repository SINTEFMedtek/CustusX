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
  mTextBrowser->append(message.getPrintableMessage());
}

}//namespace cx
