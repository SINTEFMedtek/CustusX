/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
=========================================================================*/


#include "cxConsoleWidget.h"

#include <iostream>
#include <QVBoxLayout>
#include <QMenu>
#include <QScrollBar>
#include <QContextMenuEvent>
#include "cxTypeConversions.h"

namespace cx
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

    connect(reporter(), SIGNAL(emittedMessage(Message)), this, SLOT(printMessage(Message)));

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
		"<h3>CustusX console.</h3>"
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
	if (message.getMessageLevel()==mlVOLATILE)
		return;

	this->format(message);
	this->append(message.getPrintableMessage());
}

void ConsoleWidget::lineWrappingSlot(bool checked)
{
	this->setLineWrapMode(checked ? QTextEdit::WidgetWidth : QTextEdit::NoWrap);
	//reportDebug("LineWrapping: " + qstring_cast(checked));
}

void ConsoleWidget::createTextCharFormats()
{
	mFormat[mlINFO].setForeground(Qt::black);
	mFormat[mlSUCCESS].setForeground(QColor(60, 179, 113)); // medium sea green
	mFormat[mlWARNING].setForeground(QColor(255, 140, 0)); //dark orange
	mFormat[mlERROR].setForeground(Qt::red);
	mFormat[mlDEBUG].setForeground(QColor(135, 206, 250)); //sky blue
	mFormat[mlCERR].setForeground(Qt::red);
	mFormat[mlCOUT].setForeground(Qt::darkGray);
}

void ConsoleWidget::format(Message& message)
{
	if (!mFormat.count(message.getMessageLevel()))
		return;
	this->setCurrentCharFormat(mFormat[message.getMessageLevel()]);

}

}//namespace cx
