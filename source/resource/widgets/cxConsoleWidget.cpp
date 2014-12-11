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
#include "cxEnumConverter.h"
#include "cxDefinitionStrings.h"
#include "cxSettings.h"
#include "cxStringDataAdapterXml.h"
#include "cxHelperWidgets.h"
#include "cxLabeledComboBoxWidget.h"

namespace cx
{

ConsoleWidget::ConsoleWidget(QWidget* parent) :
	BaseWidget(parent, "ConsoleWidget", "Console"),
	mLineWrappingAction(new QAction(tr("Line wrapping"), this))
{
	mMessageHistorySize = 200;
	this->setWhatsThis(this->defaultWhatsThis());

	QVBoxLayout* layout = new QVBoxLayout;
	layout->setMargin(0);
	layout->setSpacing(0);
	this->setLayout(layout);

	QHBoxLayout* buttonLayout = new QHBoxLayout;
	//	buttonLayout->setMargin(0);
	layout->addLayout(buttonLayout);

	this->addSeverityButtons(buttonLayout);
	buttonLayout->addSpacing(8);
	this->addDetailsButton(buttonLayout);

	this->createChannelSelector();

	LabeledComboBoxWidget* channelSelectorWidget = new LabeledComboBoxWidget(this, mChannelSelector);
	channelSelectorWidget->showLabel(false);
	buttonLayout->addSpacing(8);
	buttonLayout->addWidget(channelSelectorWidget);

	buttonLayout->addStretch();

	mBrowser = new QTextBrowser(this);
	mBrowser->setReadOnly(true);
	layout->addWidget(mBrowser);

	this->createTextCharFormats();

	connect(reporter(), SIGNAL(emittedMessage(Message)), this, SLOT(receivedMessage(Message)));

	mLineWrappingAction->setCheckable(true);
	connect(mLineWrappingAction, SIGNAL(triggered(bool)), this, SLOT(lineWrappingSlot(bool)));
	this->lineWrappingSlot(mLineWrappingAction->isChecked());

	this->updateSeverityButtons();
}

ConsoleWidget::~ConsoleWidget()
{
	this->saveSeverityVisibility(mlINFO);
	this->saveSeverityVisibility(mlWARNING);
	this->saveSeverityVisibility(mlERROR);
	this->saveSeverityVisibility(mlDEBUG);

	settings()->setValue("console/showDetails", mDetailsAction->isChecked());
}

QString ConsoleWidget::defaultWhatsThis() const
{
	return "<html>"
		"<h3>CustusX console.</h3>"
		"<p>Display device for system administration messages.</p>"
		"<p><i>Right click for addition options.</i></p>"
		"</html>";
}

void ConsoleWidget::saveSeverityVisibility(MESSAGE_LEVEL severity)
{
	QAction* action = mSeverityActions[severity];
	QString uid = QString("console/show%1").arg(enum2string(severity));
	settings()->setValue(uid, action->isChecked());
}

void ConsoleWidget::loadSeverityVisibility(MESSAGE_LEVEL severity)
{
	QString uid = QString("console/show%1").arg(enum2string<MESSAGE_LEVEL>(severity));
	bool value = settings()->value(uid, true).toBool();

	QAction* action = mSeverityActions[severity];
	action->blockSignals(true);
	action->setChecked(value);
	action->blockSignals(false);
}

void ConsoleWidget::addSeverityButtons(QBoxLayout* buttonLayout)
{
	this->addSeverityButton(buttonLayout, mlERROR, "window-close-3.png", "Errors", "Show Error Messages");
	this->addSeverityButton(buttonLayout, mlWARNING, "dialog-warning-panel.png", "Warnings", "Show Warning Messages");
	this->addSeverityButton(buttonLayout, mlINFO, "dialog-information-4.png", "Info", "Show Info Messages");
	this->addSeverityButton(buttonLayout, mlDEBUG, "script-error.png", "Debug", "Show Debug Messages");
}

void ConsoleWidget::createChannelSelector()
{
	QString defval = "console";
	mChannels << "all";
	mChannels << defval;

	StringDataAdapterXmlPtr retval;
	retval = StringDataAdapterXml::initialize("ChannelSelector",
											 "", "Log Channel to display",
											 defval, mChannels, QDomNode());
	connect(retval.get(), &StringDataAdapter::changed, this, &ConsoleWidget::onSeverityButtonsChanged);
	mChannelSelector = retval;
}

void ConsoleWidget::addDetailsButton(QBoxLayout* buttonLayout)
{
	QIcon icon(":/icons/open_icon_library/system-run-5.png");
	QAction* action = this->createAction(this,
									   icon,
									   "Details", "Show detailed info on each log entry",
									   SLOT(onSeverityButtonsChanged()),
									   buttonLayout, new CXSmallToolButton());
	action->setCheckable(true);

	bool value = settings()->value("console/showDetails").toBool();
	action->blockSignals(true);
	action->setChecked(value);
	action->blockSignals(false);

	mDetailsAction = action;
}

void ConsoleWidget::addSeverityButton(QBoxLayout* buttonLayout, MESSAGE_LEVEL severity, QString iconname, QString text, QString help)
{
	QIcon icon(QString(":/icons/message_levels/%1").arg(iconname));
	QAction* action = this->createAction(this,
									   icon,
									   text, help,
									   SLOT(onSeverityButtonsChanged()),
									   buttonLayout, new CXSmallToolButton());
	action->setCheckable(true);
	mSeverityActions[severity] = action;

//	connect(mBrowser, SIGNAL(backwardAvailable(bool)), back, SLOT(setEnabled(bool)));
//	connect(mBrowser, SIGNAL(forwardAvailable(bool)), forward, SLOT(setEnabled(bool)));
}

void ConsoleWidget::updateSeverityButtons()
{
	// set state of buttons according to input
	this->loadSeverityVisibility(mlINFO);
	this->loadSeverityVisibility(mlWARNING);
	this->loadSeverityVisibility(mlERROR);
	this->loadSeverityVisibility(mlDEBUG);
}

void ConsoleWidget::onSeverityButtonsChanged()
{
	// filter the output accoring to checkstates
	mBrowser->clear();

	for (std::list<Message>::iterator i=mMessageHistory.begin(); i!=mMessageHistory.end(); ++i)
	{
		this->printMessage(*i);
	}

//	CX_LOG_DEBUG("debug message");
//	CX_LOG_INFO("info message");
//	CX_LOG_WARNING("warning message");
//	CX_LOG_ERROR("error message");
}


void ConsoleWidget::contextMenuEvent(QContextMenuEvent* event)
{
	QMenu *menu = mBrowser->createStandardContextMenu();
	menu->addSeparator();
	menu->addAction(mLineWrappingAction);
	menu->exec(event->globalPos());
	delete menu;
}

void ConsoleWidget::showEvent(QShowEvent* event)
{
	mBrowser->horizontalScrollBar()->setValue(mBrowser->horizontalScrollBar()->minimum());
}

bool ConsoleWidget::isActive(const Message& message)
{
	if (!isActiveChannel(message))
		return false;

	switch (message.getMessageLevel())
	{
	case mlCERR :
	case mlERROR :
	case mlSUCCESS : return mSeverityActions[mlERROR]->isChecked();
	case mlWARNING : return mSeverityActions[mlWARNING]->isChecked();
	case mlINFO : return mSeverityActions[mlINFO]->isChecked();
	case mlCOUT :
	case mlDEBUG : return mSeverityActions[mlDEBUG]->isChecked();
	default: return false;
	}

}

bool ConsoleWidget::isActiveChannel(const Message& message) const
{
	if (mChannelSelector->getValue() == "all")
		return true;
	if (message.mChannel == mChannelSelector->getValue())
		return true;
	return false;
}

void ConsoleWidget::receivedMessage(Message message)
{
	if (message.getMessageLevel()==mlVOLATILE)
		return;

	if (!mChannels.count(message.mChannel))
	{
		mChannels.append(message.mChannel);
		mChannelSelector->setValueRange(mChannels);
	}

	mMessageHistory.push_back(message);
	while (mMessageHistory.size() > mMessageHistorySize)
	{
		mMessageHistory.pop_front();
	}

	this->printMessage(message);
}

void ConsoleWidget::printMessage(const Message& message)
{
	if (!this->isActive(message))
		return;

	this->format(message);

	if (mDetailsAction->isChecked())
		mBrowser->append(message.getPrintableMessage());
	else
		mBrowser->append(this->getCompactMessage(message));
}

QString ConsoleWidget::getCompactMessage(Message message)
{
	if(message.mMessageLevel == mlRAW)
		return message.mText;

	QString retval;
	retval= QString("[%1] %2")
							.arg(message.mTimeStamp.toString("hh:mm"))
							.arg(message.mText);

	return retval;

}

void ConsoleWidget::lineWrappingSlot(bool checked)
{
	mBrowser->setLineWrapMode(checked ? QTextEdit::WidgetWidth : QTextEdit::NoWrap);
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

void ConsoleWidget::format(const Message& message)
{
	if (!mFormat.count(message.getMessageLevel()))
		return;
	mBrowser->setCurrentCharFormat(mFormat[message.getMessageLevel()]);

}

}//namespace cx
