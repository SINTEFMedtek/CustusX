#include "cxConsoleWidget.h"

#include <iostream>
#include <QTextBrowser>
#include <QVBoxLayout>

namespace cx
{

ConsoleWidget::ConsoleWidget(QWidget* parent) :
    WhatsThisWidget(parent)
{
  this->setObjectName("ConsoleWidget");
  this->setWindowTitle("Console");
  this->setWhatsThis(this->defaultWhatsThis());

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

QString ConsoleWidget::defaultWhatsThis() const
{
  return "<html>"
         "<h3>CustusX3 console.</h3>"
         "<p>Display device for system administration messages.</p>"
         "<p><i>Right click for addition options.</i></p>"
         "</html>";
}

void ConsoleWidget::printMessage(Message message)
{
  this->format(message);

  mTextBrowser->append(message.getPrintableMessage());
}

void ConsoleWidget::createTextCharFormats()
{
  mFormat[ssc::mlINFO].setForeground(Qt::black);
  mFormat[ssc::mlWARNING].setForeground(QColor(255,140,0));  //dark orange
  mFormat[ssc::mlERROR].setForeground(Qt::red);
  mFormat[ssc::mlDEBUG].setForeground(QColor(135,206,250)); //sky blue
  mFormat[ssc::mlCERR].setForeground(Qt::red);
  mFormat[ssc::mlCOUT].setForeground(QColor(34,139,34));
}

void ConsoleWidget::format(Message& message)
{
  if (!mFormat.count(message.getMessageLevel()))
    return;
  mTextBrowser->setCurrentCharFormat(mFormat[message.getMessageLevel()]);

}

}//namespace cx
