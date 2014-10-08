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

#include <QTextBrowser>
#include <QTextCharFormat>
#include "cxReporter.h"

class QContextMenuEvent;
class QAction;

namespace cx
{
/**\brief Widget for displaying status messages.
 *
 * \date 24. aug. 2010
 * \author: Janne Beate Bakeng, SINTEF Medisinsk Teknologi
 *
 * \ingroup cx_resource_widgets
 */
class cxResourceWidgets_EXPORT ConsoleWidget: public QTextBrowser
{
Q_OBJECT

public:
	ConsoleWidget(QWidget* parent);
	~ConsoleWidget();
	virtual QString defaultWhatsThis() const;

protected slots:
	void contextMenuEvent(QContextMenuEvent* event);
	virtual void showEvent(QShowEvent* event); ///<updates internal info before showing the widget

private slots:
	void printMessage(Message message); ///< prints the message into the console
	void lineWrappingSlot(bool checked);

private:
	void createTextCharFormats(); ///< sets up the formating rules for the message levels
	void format(Message& message); ///< formats the text to suit the message level

	QAction* mLineWrappingAction;

	std::map<MESSAGE_LEVEL, QTextCharFormat> mFormat;
};
} // namespace cx
#endif /* CXCONSOLEWIDGET_H_ */
