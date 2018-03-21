/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
