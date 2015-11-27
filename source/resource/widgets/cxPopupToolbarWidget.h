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
#ifndef CXPOPUPTOOLBARWIDGET_H
#define CXPOPUPTOOLBARWIDGET_H

#include <QFrame>
#include "cxResourceWidgetsExport.h"

class QToolButton;
class QHBoxLayout;

namespace cx
{

class cxResourceWidgets_EXPORT PopupButton : public QFrame
{
	Q_OBJECT
public:
	PopupButton(QWidget *parent = NULL);
	bool getShowPopup() const;
	void setShowPopup(bool val);

signals:
	void popup(bool show);
private slots:
	void onTriggered();
private:
	QAction* mAction;
	QToolButton* mShowHeaderButton;
};

/**
 * A toolbar that pops up inside a layout,
 * controlled by a popup button floating
 * over the parent widget.
 *
 * Note: In order to get this working, set the parent layout
 * margin and spacing to zero.
 *
 * Add buttons to the toolbar by adding to getToolbar()
 *
 */
class cxResourceWidgets_EXPORT PopupToolbarWidget : public QWidget
{
	Q_OBJECT
public:
	PopupToolbarWidget(QWidget* parent);
	/**
	 * Return true if the popup is activated, i.e. the popup
	 * is pressed and the toolbar is visible.
	 *
	 * Default: false.
	 */
	bool popupIsVisible() const;
	/**
	 * Show or hide the popup.
	 */
	void setPopupVisible(bool val);
	/**
	 * Return an empty widget inside the toolbar. It can
	 * be filled with buttons or anything else.
	 */
	QWidget* getToolbar();
	/**
	 * Call after parent gui construction is complete -
	 * required because we want to draw the popup button
	 * on top of the parent layout.
	 * Hack!
	 */
	void refresh() { this->onPopup(); }
signals:
	/**
	 * emitted on popup show/hide.
	 */
	void popup(bool show);
private:
	void onPopup();
	QWidget* mButtonWidget;
	QHBoxLayout* mControlLayout;
	PopupButton* mShowControlsButton;
};

} // namespace cx

#endif // CXPOPUPTOOLBARWIDGET_H
