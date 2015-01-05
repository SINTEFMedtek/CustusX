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
#include <QTableWidget>
#include "cxLogMessageFilter.h"
#include "cxDataLocations.h"
#include <QHeaderView>
#include <QStackedLayout>
#include <QApplication>
#include <QClipboard>

namespace cx
{

LogMessageDisplayWidget::LogMessageDisplayWidget(QWidget *parent)
{
	this->createTextCharFormats(); ///< sets up the formating rules for the message levels
}

void LogMessageDisplayWidget::createTextCharFormats()
{
	mFormat[mlINFO].setForeground(Qt::black);
	mFormat[mlSUCCESS].setForeground(QColor(60, 179, 113)); // medium sea green
	mFormat[mlWARNING].setForeground(QColor(255, 140, 0)); //dark orange
	mFormat[mlERROR].setForeground(Qt::red);
	mFormat[mlDEBUG].setForeground(QColor(135, 206, 250)); //sky blue
	mFormat[mlCERR].setForeground(Qt::red);
	mFormat[mlCOUT].setForeground(Qt::darkGray);
}

///--------------------------------------------------------
///--------------------------------------------------------
///--------------------------------------------------------

class MyTableWidget : public QTableWidget
{
public:
	MyTableWidget(QWidget* parent=NULL) : QTableWidget(parent) {}
	virtual ~MyTableWidget() {}
	virtual void keyPressEvent(QKeyEvent* event);
};

//source: http://stackoverflow.com/questions/3135737/copying-part-of-qtableview
void MyTableWidget::keyPressEvent(QKeyEvent* event)
{
	// If Ctrl-C typed
	// Or use event->matches(QKeySequence::Copy)
	if (event->key() == Qt::Key_C && (event->modifiers() & Qt::ControlModifier))
	{
		QModelIndexList cells = selectedIndexes();
		qSort(cells); // Necessary, otherwise they are in column order

		QString text;
		int currentRow = 0; // To determine when to insert newlines
		foreach (const QModelIndex& cell, cells) {
			if (text.length() == 0) {
				// First item
			} else if (cell.row() != currentRow) {
				// New row
				text += '\n';
			} else {
				// Next cell
				text += '\t';
			}
			currentRow = cell.row();
			text += cell.data().toString();
		}

		QApplication::clipboard()->setText(text);
	}
}

///--------------------------------------------------------

DetailedLogMessageDisplayWidget::DetailedLogMessageDisplayWidget(QWidget *parent, XmlOptionFile options) :
	LogMessageDisplayWidget(parent),
	mOptions(options)
{
	mTable = new MyTableWidget(this);
	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->setMargin(0);
	this->setLayout(layout);
	layout->addWidget(mTable);

	mTable->setShowGrid(false);
	mTable->setTextElideMode(Qt::ElideLeft);
	mTable->setWordWrap(false);

	mTable->setColumnCount(6);
	mTable->setRowCount(0);
	mTable->setHorizontalHeaderLabels(QStringList() << "time" << "source" << "function" << "thread" << "level" << "description");
	mTable->setSelectionBehavior(QAbstractItemView::SelectRows);
	mTable->verticalHeader()->hide();

	QFontMetrics metric(this->font());
	mTable->setColumnWidth(0, metric.width("00:00:00.000xx"));
	mTable->setColumnWidth(1, metric.width("cxSourceFile.cpp:333xx"));
	mTable->setColumnWidth(2, metric.width("function()xx"));
	mTable->setColumnWidth(3, metric.width("mainxx"));
	mTable->setColumnWidth(4, metric.width("WARNINGxx"));
	mTable->horizontalHeader()->setStretchLastSection(true);

	for (int i=0; i<mTable->horizontalHeader()->count(); ++i)
	{
		XmlOptionItem headerItem("headerwidth_"+QString::number(i), mOptions.getElement());
		int value = headerItem.readValue("-1").toInt();
		if (value<0)
			continue;
		mTable->setColumnWidth(i, value);
	}
}

DetailedLogMessageDisplayWidget::~DetailedLogMessageDisplayWidget()
{
	for (int i=0; i<mTable->horizontalHeader()->count(); ++i)
	{
		XmlOptionItem headerItem("headerwidth_"+QString::number(i), mOptions.getElement());
		headerItem.writeValue(QString::number(mTable->columnWidth(i)));
	}
}

void DetailedLogMessageDisplayWidget::clear()
{
	mTable->setRowCount(0);
}

void DetailedLogMessageDisplayWidget::normalize()
{
	mTable->horizontalScrollBar()->setValue(mTable->horizontalScrollBar()->minimum());
}

void DetailedLogMessageDisplayWidget::add(const Message& message)
{
	QFontMetrics metric(this->font());
	int textLineHeight = metric.lineSpacing();
//	int h2 = mTable->rowHeight(0);

	int row = mTable->rowCount();
	mTable->insertRow(row);
//	std::cout << "insert row " << row << std::endl;
	mTable->setRowHeight(row, textLineHeight);

//	this->addItem(0, QString::number(height), message);
//	this->addItem(1, QString::number(h2), message);

	QTableWidgetItem* item = NULL;

	QString timestamp = message.getTimeStamp().toString("hh:mm:ss.zzz");
	item = this->addItem(0, timestamp, message);

	QString source;
	if (!message.mSourceFile.isEmpty())
		source = QString("%1:%2").arg(message.mSourceFile).arg(message.mSourceLine);
	item = this->addItem(1, source, message);
//	item->setTextAlignment(Qt::AlignRight);

	QString function = message.mSourceFunction;
	item = this->addItem(2, function, message);

	QString thread = message.mThread;
	item = this->addItem(3, thread, message);

	QString level = enum2string(message.getMessageLevel());
	item = this->addItem(4, level, message);

	QString desc = message.getText();
	item = this->addItem(5, desc, message);
}

QTableWidgetItem* DetailedLogMessageDisplayWidget::addItem(int column, QString text, const Message& message)
{
	int row = mTable->rowCount();

	QTableWidgetItem* item = new QTableWidgetItem(text);
	item->setStatusTip(text);
	item->setToolTip(text);
	item->setForeground(mFormat[message.getMessageLevel()].foreground());
	item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
	mTable->setItem(row-1, column, item);
	return item;
}

///--------------------------------------------------------
///--------------------------------------------------------
///--------------------------------------------------------

SimpleLogMessageDisplayWidget::SimpleLogMessageDisplayWidget(QWidget *parent) :
	LogMessageDisplayWidget(parent)
{
	mBrowser = new QTextBrowser(this);
	mBrowser->setReadOnly(true);
	mBrowser->setLineWrapMode(QTextEdit::NoWrap);
	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->setMargin(0);
	this->setLayout(layout);
	layout->addWidget(mBrowser);
	this->setTail();
}

void SimpleLogMessageDisplayWidget::clear()
{
	mBrowser->clear();
	this->setTail();
}

void SimpleLogMessageDisplayWidget::normalize()
{
	mBrowser->horizontalScrollBar()->setValue(mBrowser->horizontalScrollBar()->minimum());
}

void SimpleLogMessageDisplayWidget::add(const Message& message)
{
	this->format(message);

//	if (mDetailsAction->isChecked())
//		mBrowser->append(message.getPrintableMessage());
//	else

	bool tail = this->isTailing();

	mBrowser->append(this->getCompactMessage(message));

//	mBrowser->horizontalScrollBar()->setValue(mBrowser->horizontalScrollBar()->minimum());
	if (tail)
		this->setTail();
}

bool SimpleLogMessageDisplayWidget::isTailing() const
{
	bool tail = mBrowser->verticalScrollBar()->maximum() == mBrowser->verticalScrollBar()->value();
	return tail;
}
void SimpleLogMessageDisplayWidget::setTail()
{
	mBrowser->verticalScrollBar()->setValue(mBrowser->verticalScrollBar()->maximum());
}


QString SimpleLogMessageDisplayWidget::getCompactMessage(Message message)
{
	if(message.mMessageLevel == mlRAW)
		return message.mText;

	QString retval;
	retval= QString("[%1] %2")
			.arg(message.mTimeStamp.toString("hh:mm"))
			.arg(message.mText);
	return retval;
}

void SimpleLogMessageDisplayWidget::format(const Message& message)
{
	if (!mFormat.count(message.getMessageLevel()))
		return;
	mBrowser->setCurrentCharFormat(mFormat[message.getMessageLevel()]);
}

///--------------------------------------------------------
///--------------------------------------------------------
///--------------------------------------------------------

ConsoleWidget::ConsoleWidget(QWidget* parent, QString uid, QString name) :
	BaseWidget(parent, uid, name),
	mLineWrappingAction(new QAction(tr("Line wrapping"), this)),
	mSeverityAction(NULL),
	mMessagesWidget(NULL)
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

	mStackedLayout = new QStackedLayout;
	mStackedLayout->setMargin(0);
	layout->addLayout(mStackedLayout);


//	mMessagesWidget = new SimpleLogMessageDisplayWidget(this);
//	mMessagesWidget = new DetailedLogMessageDisplayWidget(this);
//	mStackedLayout->addWidget(mMessagesWidget);

//	mLog = LogFileWatcher::create();

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

void ConsoleWidget::setDetails(bool on)
{
	mDetailsAction->setChecked(on);
}

void ConsoleWidget::addSeverityButtons(QBoxLayout* buttonLayout)
{
	QAction* actionUp = this->createAction(this,
										 QIcon(":/icons/open_icon_library/zoom-in-3.png"),
										 "More", "More detailed log output",
										 SLOT(onSeverityDown()),
										 buttonLayout, new CXSmallToolButton());

	this->addSeverityIndicator(buttonLayout);

	QAction* actionDown = this->createAction(this,
										 QIcon(":/icons/open_icon_library/zoom-out-3.png"),
											 "Less ", "Less detailed log output",
										   SLOT(onSeverityUp()),
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

	this->setWindowTitle("Console: " + mChannelSelector->getValue());
	this->selectMessagesWidget();

	// reset content of browser
	QTimer::singleShot(0, this, SLOT(clearTable())); // let the messages recently emitted be processed before clearing

	mMessageListener->restart();
}

void ConsoleWidget::selectMessagesWidget()
{
	if (mMessagesWidget && (mMessagesWidget->getType()==this->getDetailTypeFromButton()))
		return;

	if (mMessagesWidget)
	{
		// remove
		mStackedLayout->takeAt(0);
		delete mMessagesWidget;
	}

	if (this->getDetailTypeFromButton()=="detail")
	{
		mMessagesWidget = new DetailedLogMessageDisplayWidget(this, mOptions);
	}
	else
	{
		mMessagesWidget = new SimpleLogMessageDisplayWidget(this);
	}

	mStackedLayout->addWidget(mMessagesWidget);
}

QString ConsoleWidget::getDetailTypeFromButton() const
{
	if (mDetailsAction->isChecked())
		return "detail";
	else
		return "simple";
}

void ConsoleWidget::clearTable()
{
	mMessagesWidget->clear();
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
//	QMenu *menu = mBrowser->createStandardContextMenu();
//	menu->addSeparator();
//	menu->addAction(mLineWrappingAction);
//	menu->exec(event->globalPos());
//	delete menu;
}

void ConsoleWidget::showEvent(QShowEvent* event)
{
	mMessagesWidget->normalize();
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
	mMessagesWidget->add(message);
}

void ConsoleWidget::lineWrappingSlot(bool checked)
{
//	mBrowser->setLineWrapMode(checked ? QTextEdit::WidgetWidth : QTextEdit::NoWrap);
}

}//namespace cx
