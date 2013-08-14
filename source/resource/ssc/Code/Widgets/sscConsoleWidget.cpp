// This file is part of SSC,
// a C++ Library supporting Image Guided Therapy Applications.
//
// Copyright (C) 2008- SINTEF Medical Technology
// Copyright (C) 2008- Sonowand AS
//
// SSC is owned by SINTEF Medical Technology and Sonowand AS,
// hereafter named the owners. Each particular piece of code
// is owned by the part that added it to the library.
// SSC source code and binaries can only be used by the owners
// and those with explicit permission from the owners.
// SSC shall not be distributed to anyone else.
//
// SSC is distributed WITHOUT ANY WARRANTY; without even
// the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE.
//
// See sscLicense.txt for more information.

#include "sscConsoleWidget.h"

#include <iostream>
#include <QVBoxLayout>
#include <QMenu>
#include <QScrollBar>
#include <QContextMenuEvent>
#include "sscTypeConversions.h"

namespace ssc
{

ConsoleWidget::ConsoleWidget(QWidget* parent) :
	QTextBrowser(parent), mLineWrappingAction(new QAction(tr("Line wrapping"), this))
{
	this->setObjectName("ConsoleWidget");
	this->setWindowTitle("Console");
	this->setWhatsThis(this->defaultWhatsThis());

	QVBoxLayout* layout = new QVBoxLayout;
	this->setLayout(layout);

	this->setReadOnly(true);
	this->createTextCharFormats();

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

void ConsoleWidget::showEvent(QShowEvent* event)
{
	this->horizontalScrollBar()->setValue(this->horizontalScrollBar()->minimum());
}

void ConsoleWidget::printMessage(Message message)
{
	this->format(message);

	this->append(message.getPrintableMessage());
}

void ConsoleWidget::lineWrappingSlot(bool checked)
{
	this->setLineWrapMode(checked ? QTextEdit::WidgetWidth : QTextEdit::NoWrap);
	//ssc::messageManager()->sendDebug("LineWrapping: " + qstring_cast(checked));
}

void ConsoleWidget::createTextCharFormats()
{
	mFormat[ssc::mlINFO].setForeground(Qt::black);
	mFormat[ssc::mlSUCCESS].setForeground(QColor(60, 179, 113)); // medium sea green
	mFormat[ssc::mlWARNING].setForeground(QColor(255, 140, 0)); //dark orange
	mFormat[ssc::mlERROR].setForeground(Qt::red);
	mFormat[ssc::mlDEBUG].setForeground(QColor(135, 206, 250)); //sky blue
	mFormat[ssc::mlCERR].setForeground(Qt::red);
	mFormat[ssc::mlCOUT].setForeground(Qt::darkGray);
}

void ConsoleWidget::format(Message& message)
{
	if (!mFormat.count(message.getMessageLevel()))
		return;
	this->setCurrentCharFormat(mFormat[message.getMessageLevel()]);

}

}//namespace ssc
