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
#include "cxMessageListener.h"
#include "cxEnumConverter.h"
#include "cxUtilHelpers.h"
#include <QTimer>
#include <QThread>
#include "cxLogMessageFilter.h"
#include "cxDataLocations.h"

namespace cx
{

ConsoleWidget::ConsoleWidget(QWidget* parent, QString uid, QString name) :
	BaseWidget(parent, uid, name),
	mLineWrappingAction(new QAction(tr("Line wrapping"), this)),
	mSeverityAction(NULL)
{
	this->setWhatsThis(this->defaultWhatsThis());
	mOptions = XmlOptionFile(DataLocations::getXmlSettingsFile()).descend(this->objectName());

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
	buttonLayout->setStretch(buttonLayout->count()-1, 0);

	buttonLayout->addStretch(1);

	mBrowser = new QTextBrowser(this);
	mBrowser->setReadOnly(true);
	layout->addWidget(mBrowser);

	this->createTextCharFormats();

	mMessageListener = MessageListener::create();
	mMessageFilter.reset(new MessageFilterConsole);
	mMessageListener->installFilter(mMessageFilter);
	connect(mMessageListener.get(), &MessageListener::newMessage, this, &ConsoleWidget::receivedMessage);

	QString defVal = enum2string<LOG_SEVERITY>(msINFO);
	XmlOptionItem showLevelItem("showLevel", mOptions.getElement());
	LOG_SEVERITY value = string2enum<LOG_SEVERITY>(showLevelItem.readValue(defVal));
	mMessageFilter->setLowestSeverity(value);

	mMessageFilter->setActiveChannel(mChannelSelector->getValue());
	mMessageListener->installFilter(mMessageFilter);

	mLineWrappingAction->setCheckable(true);
	connect(mLineWrappingAction, SIGNAL(triggered(bool)), this, SLOT(lineWrappingSlot(bool)));
	this->lineWrappingSlot(mLineWrappingAction->isChecked());

	this->updateUI();
}

ConsoleWidget::~ConsoleWidget()
{
	XmlOptionItem showLevelItem("showLevel", mOptions.getElement());
	QString levelString = enum2string<LOG_SEVERITY>(mMessageFilter->getLowestSeverity());
	showLevelItem.writeValue(levelString);

	XmlOptionItem showDetailsItem("showDetails", mOptions.getElement());
	showDetailsItem.writeVariant(mDetailsAction->isChecked());
}

QString ConsoleWidget::defaultWhatsThis() const
{
	return "<html>"
			"<h3>CustusX console.</h3>"
			"<p>Display device for system administration messages.</p>"
			"<p><i>Right click for addition options.</i></p>"
			"</html>";
}

void ConsoleWidget::addSeverityButtons(QBoxLayout* buttonLayout)
{
	QAction* actionUp = this->createAction(this,
										 QIcon(":/icons/open_icon_library/zoom-in-3.png"),
										 "More", "More detailed log output",
										 SLOT(onSeverityUp()),
										 buttonLayout, new CXSmallToolButton());

	this->addSeverityIndicator(buttonLayout);

	QAction* actionDown = this->createAction(this,
										 QIcon(":/icons/open_icon_library/zoom-out-3.png"),
											 "Less ", "Less detailed log output",
										   SLOT(onSeverityDown()),
										 buttonLayout, new CXSmallToolButton());
}

void ConsoleWidget::addSeverityIndicator(QBoxLayout* buttonLayout)
{
	QAction* action = new QAction(QIcon(""), "Severity", this);
	mSeverityAction = action;
	QString help = "Lowest displayed log severity";
	action->setStatusTip(help);
	action->setWhatsThis(help);
	action->setToolTip(help);
	QToolButton* button = new CXSmallToolButton();
	button->setDefaultAction(action);
	buttonLayout->addWidget(button);
}

void ConsoleWidget::updateSeverityIndicator()
{
	LOG_SEVERITY severity = mMessageFilter->getLowestSeverity();

	switch (severity)
	{
	case msERROR:
		this->updateSeverityIndicator("window-close-3.png", "error");
		break;
	case msWARNING:
		this->updateSeverityIndicator("dialog-warning-panel.png", "warning");
		break;
	case msINFO:
		this->updateSeverityIndicator("dialog-information-4.png", "info");
		break;
	case msDEBUG:
		this->updateSeverityIndicator("script-error.png", "debug");
		break;
	default:
		this->updateSeverityIndicator("script-error.png", "");
		break;
	}
}

void ConsoleWidget::updateSeverityIndicator(QString iconname, QString help)
{
	QIcon icon(QString(":/icons/message_levels/%1").arg(iconname));
	mSeverityAction->setIcon(icon);

	help = QString("Current log level is %1").arg(help);
	mSeverityAction->setStatusTip(help);
	mSeverityAction->setToolTip(help);
}

void ConsoleWidget::onSeverityUp()
{
	this->onSeverityChange(-1);
}

void ConsoleWidget::onSeverityDown()
{
	this->onSeverityChange(+1);
}

void ConsoleWidget::onSeverityChange(int delta)
{
	LOG_SEVERITY severity = mMessageFilter->getLowestSeverity();
	int val = (int)severity + delta;
	val = constrainValue(val, 0, int(msCOUNT)-1);
	severity = static_cast<LOG_SEVERITY>(val);

	mMessageFilter->setLowestSeverity(severity);
	mMessageListener->installFilter(mMessageFilter);
	this->updateUI();
}

void ConsoleWidget::createChannelSelector()
{
	QString defval = "console";
	mChannels << "all";
	mChannels << defval;

	StringDataAdapterXmlPtr retval;
	retval = StringDataAdapterXml::initialize("ChannelSelector",
											  "", "Log Channel to display",
											  defval, mChannels, mOptions.getElement());
	connect(retval.get(), &StringDataAdapter::changed, this, &ConsoleWidget::onChannelSelectorChanged);
	mChannelSelector = retval;
}

void ConsoleWidget::addDetailsButton(QBoxLayout* buttonLayout)
{
	QIcon icon(":/icons/open_icon_library/system-run-5.png");
	QAction* action = this->createAction(this,
										 icon,
										 "Details", "Show detailed info on each log entry",
										 SLOT(updateUI()),
										 buttonLayout, new CXSmallToolButton());
	action->setCheckable(true);

	XmlOptionItem showDetailsItem("showDetails", mOptions.getElement());
	bool value = showDetailsItem.readVariant(false).toBool();
	action->blockSignals(true);
	action->setChecked(value);
	action->blockSignals(false);

	mDetailsAction = action;
}

void ConsoleWidget::updateUI()
{
	this->updateSeverityIndicator();

	// reset content of browser
//	mBrowser->clear();
	QTimer::singleShot(0, mBrowser, SLOT(clear())); // let the messages recently emitted be processed before clearing

	mMessageListener->restart();
}

void ConsoleWidget::onChannelSelectorChanged()
{
	mChannelSelector->blockSignals(true);

	mMessageFilter->setActiveChannel(mChannelSelector->getValue());
	mMessageListener->installFilter(mMessageFilter);
	this->updateUI();

	mChannelSelector->blockSignals(false);
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

void ConsoleWidget::receivedMessage(Message message)
{
	if (!mChannels.count(message.mChannel))
	{
		mChannels.append(message.mChannel);
		mChannelSelector->setValueRange(mChannels);
	}

	this->printMessage(message);
}

void ConsoleWidget::printMessage(const Message& message)
{
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
