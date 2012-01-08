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

#ifndef SSCCONSOLEWIDGET_H_
#define SSCCONSOLEWIDGET_H_

#include <QTextBrowser>
#include <QTextCharFormat>
#include "sscMessageManager.h"

class QContextMenuEvent;
class QAction;

namespace ssc
{
/**\brief Widget for displaying status messages.
 *
 * \date 24. aug. 2010
 * \author: Janne Beate Bakeng, SINTEF Medisinsk Teknologi
 *
 * \ingroup sscWidget
 */
class ConsoleWidget: public QTextBrowser
{
	typedef ssc::Message Message;

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

	std::map<ssc::MESSAGE_LEVEL, QTextCharFormat> mFormat;
};
} // namespace ssc
#endif /* SSCCONSOLEWIDGET_H_ */
