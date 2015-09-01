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
#include "cxStringProperty.h"
#include "cxHelperWidgets.h"
#include "cxLabeledComboBoxWidget.h"
#include "cxMessageListener.h"
#include "cxEnumConverter.h"
#include "cxUtilHelpers.h"
#include <QTimer>
#include <QThread>
#include <QTableWidget>
#include "cxLogMessageFilter.h"
#include <QHeaderView>
#include <QStackedLayout>
#include <QApplication>
#include <QClipboard>
#include "cxNullDeleter.h"
#include <QPushButton>
#include "cxProfile.h"
#include "cxTime.h"

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
	else
	{
		event->ignore();
	}

	QTableWidget::keyPressEvent(event);
}

///--------------------------------------------------------

DetailedLogMessageDisplayWidget::DetailedLogMessageDisplayWidget(QWidget *parent, XmlOptionFile options) :
	LogMessageDisplayWidget(parent),
	mOptions(options),
	mScrollToBottomEnabled(true)
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

	int textLineHeight = metric.lineSpacing();
	mTable->verticalHeader()->setDefaultSectionSize(textLineHeight);
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
	int row = mTable->rowCount();
	mTable->insertRow(row);

	QString timestamp = message.getTimeStamp().toString("hh:mm:ss.zzz");
	this->addItem(0, timestamp, message);

	QString source;
	if (!message.mSourceFile.isEmpty())
		source = QString("%1:%2").arg(message.mSourceFile).arg(message.mSourceLine);
	this->addItem(1, source, message);

	QString function = message.mSourceFunction;
	this->addItem(2, function, message);

	QString thread = message.mThread;
	this->addItem(3, thread, message);

	QString level = enum2string(message.getMessageLevel());
	this->addItem(4, level, message);

	QString desc = message.getText();
	this->addItem(5, desc, message);

	this->scrollToBottom();
}

void DetailedLogMessageDisplayWidget::setScrollToBottom(bool on)
{
	mScrollToBottomEnabled = on;
	this->scrollToBottom();
}

void DetailedLogMessageDisplayWidget::showHeader(bool on)
{
	mTable->horizontalHeader()->setVisible(on);
}

void DetailedLogMessageDisplayWidget::scrollToBottom()
{
	if (mScrollToBottomEnabled)
		QTimer::singleShot(0, mTable, SLOT(scrollToBottom()));
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
	LogMessageDisplayWidget(parent),
	mScrollToBottomEnabled(true)
{
	mBrowser = new QTextBrowser(this);
	mBrowser->setReadOnly(true);
	mBrowser->setLineWrapMode(QTextEdit::NoWrap);
	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->setMargin(0);
	this->setLayout(layout);
	layout->addWidget(mBrowser);
	this->scrollToBottom();
}

void SimpleLogMessageDisplayWidget::clear()
{
	mBrowser->clear();
	this->scrollToBottom();
}

void SimpleLogMessageDisplayWidget::normalize()
{
	mBrowser->horizontalScrollBar()->setValue(mBrowser->horizontalScrollBar()->minimum());
}

void SimpleLogMessageDisplayWidget::add(const Message& message)
{
	this->format(message);

	mBrowser->append(this->getCompactMessage(message));

	this->scrollToBottom();
}

void SimpleLogMessageDisplayWidget::setScrollToBottom(bool on)
{
	mScrollToBottomEnabled = on;
	this->scrollToBottom();
}

void SimpleLogMessageDisplayWidget::scrollToBottom()
{
	if (mScrollToBottomEnabled)
		mBrowser->verticalScrollBar()->setValue(mBrowser->verticalScrollBar()->maximum());
}

QString SimpleLogMessageDisplayWidget::getCompactMessage(Message message)
{
	if(message.mMessageLevel == mlRAW)
		return message.mText;

	QString retval;
	retval= QString("[%1] %2")
//			.arg(message.mTimeStamp.toString(timestampSecondsFormatNice()))
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

PopupButton::PopupButton(QWidget* parent)
{
//	this->setMouseTracking(true);
	//	this->setFrameStyle(QFrame::Box);

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->setMargin(0);
	this->setLayout(layout);

	//	QToolButton* expandButton = new QToolButton(this);
	QToolButton* expandButton = new CXSmallToolButton(this);
	mShowHeaderButton = expandButton;
	this->setFixedSize(expandButton->sizeHint());

	QAction* action = new QAction(QIcon(":icons/open_icon_library/layer-lower-3.png"), "Controls", this);
	QString tip = "Show Controls";
	action->setStatusTip(tip);
	action->setWhatsThis(tip);
	action->setToolTip(tip);
	connect(action, SIGNAL(triggered()), this, SLOT(onTriggered()));
	mAction = action;

	mShowHeaderButton->setDefaultAction(action);
	layout->addWidget(mShowHeaderButton);

	action->setCheckable(true);
}

//void PopupButton::mouseMoveEvent(QMouseEvent* event)
//{
//	std::cout << "mouse move" << std::endl;
//}

bool PopupButton::getShowPopup() const
{
	return mShowHeaderButton->isChecked();
}

void PopupButton::onTriggered()
{
	if (this->getShowPopup())
		mAction->setIcon(QIcon(":icons/open_icon_library/layer-raise-3.png"));
	else
		mAction->setIcon(QIcon(":icons/open_icon_library/layer-lower-3.png"));

	emit popup(this->getShowPopup());
}


///--------------------------------------------------------
///--------------------------------------------------------
///--------------------------------------------------------

ConsoleWidget::ConsoleWidget(QWidget* parent, QString uid, QString name, XmlOptionFile options, LogPtr log) :
	BaseWidget(parent, uid, name),
  mSeverityAction(NULL),
  mMessagesWidget(NULL),
  mMessagesLayout(NULL),
  mDetailsAction(NULL)
{
	mOptions = options;
	mLog = log;
	connect(mLog.get(), &Log::loggingFolderChanged, this, &ConsoleWidget::onLoggingFolderChanged);

	this->setModified();
}

ConsoleWidget::ConsoleWidget(QWidget* parent, QString uid, QString name) :
	BaseWidget(parent, uid, name),
	mSeverityAction(NULL),
	mMessagesWidget(NULL),
	mMessagesLayout(NULL),
	mDetailsAction(NULL)
{
	mOptions = profile()->getXmlSettings().descend(this->objectName());
	mLog = reporter();
	connect(mLog.get(), &Log::loggingFolderChanged, this, &ConsoleWidget::onLoggingFolderChanged);

	this->setModified();
}

void ConsoleWidget::prePaintEvent()
{
	if (!mMessagesLayout)
	{
		this->createUI();
	}

}

void ConsoleWidget::createUI()
{
	mSeverityAction = NULL;
	mMessagesWidget = NULL;

	this->setToolTip("Display system information, warnings and errors.");

	QVBoxLayout* layout = new QVBoxLayout;
	layout->setMargin(0);
	layout->setSpacing(0);
	this->setLayout(layout);

	mControlLayout = new QHBoxLayout;
	mControlLayout->setMargin(0);
	layout->addLayout(mControlLayout);

	mShowControlsButton = new PopupButton(this);
	mControlLayout->addWidget(mShowControlsButton);
	connect(mShowControlsButton, &PopupButton::popup, this, &ConsoleWidget::updateShowHeader);

	this->createButtonWidget();

	mControlLayout->addStretch(1);

	mMessagesLayout = new QVBoxLayout;
	mMessagesLayout->setMargin(0);
	layout->addLayout(mMessagesLayout);

	mMessageListener = MessageListener::create(mLog);
	mMessageFilter.reset(new MessageFilterConsole);
	mMessageListener->installFilter(mMessageFilter);
	connect(mMessageListener.get(), &MessageListener::newMessage, this, &ConsoleWidget::receivedMessage);
	connect(mMessageListener.get(), &MessageListener::newChannel, this, &ConsoleWidget::receivedChannel);

	QString defVal = enum2string<LOG_SEVERITY>(msINFO);
	LOG_SEVERITY value = string2enum<LOG_SEVERITY>(this->option("showLevel").readValue(defVal));
	mMessageFilter->setLowestSeverity(value);

	mMessageFilter->setActiveChannel(mChannelSelector->getValue());
	mMessageListener->installFilter(mMessageFilter);

	this->updateUI();
}

void ConsoleWidget::createButtonWidget()
{
	mButtonWidget = new QWidget(this);
	mControlLayout->addWidget(mButtonWidget);

	QHBoxLayout* buttonLayout = new QHBoxLayout;
	buttonLayout->setMargin(0);
	buttonLayout->setSpacing(0);

	mButtonWidget->setLayout(buttonLayout);

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
}

void ConsoleWidget::updateShowHeader()
{
	bool show = mShowControlsButton->getShowPopup();

	mMessagesWidget->showHeader(show);
	mButtonWidget->setVisible(show);

	if (show)
	{
		mControlLayout->insertWidget(0, mShowControlsButton);
	}
	else
	{
		// remove from layout, add to top of this
		mControlLayout->removeWidget(mShowControlsButton);
		mShowControlsButton->setParent(NULL);
		mShowControlsButton->setParent(this);
		mShowControlsButton->setVisible(true);

	}
}


XmlOptionItem ConsoleWidget::option(QString name)
{
	return XmlOptionItem(name, mOptions.getElement());
}

ConsoleWidget::~ConsoleWidget()
{
	if (!mMessageFilter)
		return;

	QString levelString = enum2string<LOG_SEVERITY>(mMessageFilter->getLowestSeverity());
	this->option("showLevel").writeValue(levelString);
	this->option("showDetails").writeVariant(mDetailsAction->isChecked());
}

void ConsoleWidget::setDetails(bool on)
{
	if (mDetailsAction)
	{
		mDetailsAction->setChecked(on);
		this->updateUI();
	}
	else
	{
		this->option("showDetails").writeVariant(on);
	}
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

	StringPropertyPtr retval;
	retval = StringProperty::initialize("ChannelSelector",
											  "", "Log Channel to display",
											  defval, mChannels, mOptions.getElement());
	connect(retval.get(), &StringPropertyBase::changed, this, &ConsoleWidget::onChannelSelectorChanged);
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

	bool value = this->option("showDetails").readVariant(false).toBool();
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
	this->updateShowHeader();

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
		mMessagesLayout->takeAt(0);
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

	mMessagesLayout->addWidget(mMessagesWidget);
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
	if (mMessagesWidget)
		mMessagesWidget->clear();
}

void ConsoleWidget::onLoggingFolderChanged()
{
	if (!mMessageFilter)
		return;
	mMessageListener->installFilter(mMessageFilter);
	this->updateUI();
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
	if (mMessagesWidget)
		mMessagesWidget->normalize();
}

void ConsoleWidget::receivedChannel(QString channel)
{
	if (!mChannels.count(channel))
	{
		mChannels.append(channel);
		mChannelSelector->setValueRange(mChannels);
	}
}

void ConsoleWidget::receivedMessage(Message message)
{
	this->receivedChannel(message.mChannel);
//	if (!mChannels.count(message.mChannel))
//	{
//		mChannels.append(message.mChannel);
//		mChannelSelector->setValueRange(mChannels);
//	}

	this->printMessage(message);
}

void ConsoleWidget::printMessage(const Message& message)
{
	if (mMessagesWidget)
		mMessagesWidget->add(message);
}

//void ConsoleWidget::lineWrappingSlot(bool checked)
//{
////	mBrowser->setLineWrapMode(checked ? QTextEdit::WidgetWidth : QTextEdit::NoWrap);
//}

}//namespace cx
