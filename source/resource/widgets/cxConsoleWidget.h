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


#ifndef CXCONSOLEWIDGET_H_
#define CXCONSOLEWIDGET_H_

#include "cxResourceWidgetsExport.h"

#include "cxBaseWidget.h"
#include "cxReporter.h"
#include <QTextBrowser>
#include <QTextCharFormat>
#include "cxStringProperty.h"
class QTableWidget;
class QTableWidgetItem;


class QContextMenuEvent;
class QAction;
class QStackedLayout;

namespace cx
{
class PopupToolbarWidget;
typedef boost::shared_ptr<class MessageListener> MessageListenerPtr;

class LogMessageDisplayWidget : public QWidget
{
public:
	LogMessageDisplayWidget(QWidget *parent);
	virtual ~LogMessageDisplayWidget() {}
	virtual void clear() = 0;
	virtual void add(const Message& message) = 0;
	virtual void normalize() = 0;
	virtual QString getType() const = 0;
	virtual void setScrollToBottom(bool on) = 0;
	virtual void showHeader(bool on) = 0;
protected:
	void createTextCharFormats(); ///< sets up the formating rules for the message levels
	std::map<MESSAGE_LEVEL, QTextCharFormat> mFormat;
};

class DetailedLogMessageDisplayWidget : public LogMessageDisplayWidget
{
public:
	DetailedLogMessageDisplayWidget(QWidget* parent, XmlOptionFile options);
	virtual ~DetailedLogMessageDisplayWidget();
	virtual void clear();
	virtual void add(const Message& message);
	virtual void normalize();
	virtual QString getType() const { return "detail";}
	virtual void setScrollToBottom(bool on);
	virtual void showHeader(bool on);

protected:
	XmlOptionFile mOptions;
	QTableWidget* mTable;
	QTableWidgetItem *addItem(int column, QString text, const Message& message);
	void scrollToBottom();
	bool mScrollToBottomEnabled;
};

class SimpleLogMessageDisplayWidget : public LogMessageDisplayWidget
{
public:
	SimpleLogMessageDisplayWidget(QWidget* parent=NULL);
	virtual ~SimpleLogMessageDisplayWidget() {}
	virtual void clear();
	virtual void add(const Message& message);
	virtual void normalize();
	virtual QString getType() const { return "simple";}
	virtual void setScrollToBottom(bool on);
	virtual void showHeader(bool on) {}

	QTextBrowser* mBrowser;
	void format(const Message &message); ///< formats the text to suit the message level
	QString getCompactMessage(Message message);
private:
	void scrollToBottom();
	bool mScrollToBottomEnabled;
};


/**\brief Widget for displaying status messages.
 *
 * \date 24. aug. 2010
 * \author: Janne Beate Bakeng, SINTEF Medisinsk Teknologi
 *
 * \ingroup cx_resource_widgets
 */
class cxResourceWidgets_EXPORT ConsoleWidget: public BaseWidget
{
Q_OBJECT

public:
	ConsoleWidget(QWidget* parent, QString uid="console_widget", QString name="Console");
	ConsoleWidget(QWidget* parent, QString uid, QString name, XmlOptionFile options, LogPtr log);
	~ConsoleWidget();

	void setDetails(bool on);

protected slots:
	void contextMenuEvent(QContextMenuEvent* event);
	virtual void showEvent(QShowEvent* event); ///<updates internal info before showing the widget

protected:
	virtual void prePaintEvent();

private slots:
	void onChannelSelectorChanged();
	void onLoggingFolderChanged();
	void receivedMessage(Message message);
	void receivedChannel(QString channel);

	void onSeverityUp();
	void onSeverityDown();
	void onSeverityChange(int delta);
	void updateUI();
	void clearTable();
	void updateShowHeader();

private:
	XmlOptionItem option(QString name);
	void createUI();
	void printMessage(const Message& message); ///< prints the message into the console
	void addSeverityButtons(QBoxLayout* buttonLayout);
	void addDetailsButton(QBoxLayout* buttonLayout);
	void createChannelSelector();
	void updateSeverityIndicator(QString iconname, QString help);
	void addSeverityIndicator(QBoxLayout* buttonLayout);
	void updateSeverityIndicator();
	QString getDetailTypeFromButton() const;
	void selectMessagesWidget();
	void createButtonWidget(QWidget *widget);

//	QAction* mLineWrappingAction;
	QAction* mSeverityAction;
	LogMessageDisplayWidget* mMessagesWidget;
	QVBoxLayout* mMessagesLayout;

	QAction* mDetailsAction;
	StringPropertyPtr mChannelSelector;
	QStringList mChannels;
	MessageListenerPtr mMessageListener;
	boost::shared_ptr<class MessageFilterConsole> mMessageFilter;
	XmlOptionFile mOptions;
	PopupToolbarWidget* mPopupWidget;

	LogPtr mLog;
};
} // namespace cx
#endif /* CXCONSOLEWIDGET_H_ */
