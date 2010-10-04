#include "cxConsoleWidget.h"

#include <iostream>
#include <QVBoxLayout>
#include <QMenu>
#include <QContextMenuEvent>
#include "sscTypeConversions.h"

namespace cx
{

ConsoleWidget::ConsoleWidget(QWidget* parent) :
  QTextBrowser(parent), mLineWrappingAction(new QAction(tr("Line wrapping"), this))
{
  this->setObjectName("ConsoleWidget");
  this->setWindowTitle("Console");
  this->setWhatsThis(this->defaultWhatsThis());

  // Set it read only
  this->setReadOnly(true);

  // Do not show frames
  this->setFrameStyle(true);
  this->setFrameStyle(QFrame::Plain);

  // Define the text formating
  this->createTextCharFormats();

  // Use layout
  QVBoxLayout* layout = new QVBoxLayout;
  this->setLayout(layout);

  connect(ssc::messageManager(), SIGNAL(emittedMessage(Message)), this, SLOT(printMessage(Message)));

  mLineWrappingAction->setCheckable(true);
  connect(mLineWrappingAction, SIGNAL(triggered(bool)), this, SLOT(lineWrappingSlot(bool)));
  this->lineWrappingSlot(mLineWrappingAction->isChecked());
}

ConsoleWidget::~ConsoleWidget()
{
}

QString ConsoleWidget::defaultWhatsThis() const
{
  return "<html>"
    "<h3>CustusX3 console.</h3>"
    "<p>Display device for system administration messages.</p>"
    "<p><i>Right click for addition options.</i></p>"
    "</html>";
}

void ConsoleWidget::contextMenuEvent(QContextMenuEvent* event)
{
  QMenu *menu = createStandardContextMenu();
  menu->addSeparator();
  menu->addAction(mLineWrappingAction);
  menu->exec(event->globalPos());
  delete menu;
}

void ConsoleWidget::printMessage(Message message)
{
  this->format(message);

  this->append(message.getPrintableMessage());
}

void ConsoleWidget::lineWrappingSlot(bool checked)
{
  this->setLineWrapMode(checked ? QTextEdit::WidgetWidth : QTextEdit::NoWrap);
  ssc::messageManager()->sendDebug("LineWrapping: " + string_cast(checked));
}

void ConsoleWidget::createTextCharFormats()
{
  mFormat[ssc::mlINFO].setForeground(Qt::black);
  mFormat[ssc::mlWARNING].setForeground(QColor(255, 140, 0)); //dark orange
  mFormat[ssc::mlERROR].setForeground(Qt::red);
  mFormat[ssc::mlDEBUG].setForeground(QColor(135, 206, 250)); //sky blue
  mFormat[ssc::mlCERR].setForeground(Qt::red);
  mFormat[ssc::mlCOUT].setForeground(QColor(34, 139, 34));
}

void ConsoleWidget::format(Message& message)
{
  if (!mFormat.count(message.getMessageLevel()))
    return;
  this->setCurrentCharFormat(mFormat[message.getMessageLevel()]);

}

}//namespace cx
