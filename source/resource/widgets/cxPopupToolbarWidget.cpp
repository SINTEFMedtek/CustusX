/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#include "cxPopupToolbarWidget.h"

#include <QHBoxLayout>
#include <QToolButton>
#include <QIcon>
#include "cxBaseWidget.h"
#include "cxLogger.h"

namespace cx
{


PopupButton::PopupButton(QWidget* parent)
{
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

bool PopupButton::getShowPopup() const
{
	return mAction->isChecked();
}

void PopupButton::setShowPopup(bool val)
{
	mAction->setChecked(val);
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

PopupToolbarWidget::PopupToolbarWidget(QWidget* parent) :
	QWidget(parent)
{
	mControlLayout = new QHBoxLayout(this);
	mControlLayout->setMargin(0);

	mShowControlsButton = new PopupButton(this->parentWidget());
	mControlLayout->addWidget(mShowControlsButton);
	connect(mShowControlsButton, &PopupButton::popup, this, &PopupToolbarWidget::onPopup);

	mButtonWidget = new QWidget(this);
	mControlLayout->addWidget(mButtonWidget);

	this->onPopup();
}

QWidget* PopupToolbarWidget::getToolbar()
{
	return mButtonWidget;
}

bool PopupToolbarWidget::popupIsVisible() const
{
	return mButtonWidget->isVisible();
	//	return mShowControlsButton->getShowPopup();
}

void PopupToolbarWidget::setPopupVisible(bool val)
{
	mShowControlsButton->setShowPopup(val);
}

void PopupToolbarWidget::onPopup()
{
	bool show = mShowControlsButton->getShowPopup();

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
		mShowControlsButton->setParent(this->parentWidget());
		mShowControlsButton->setVisible(true);
	}

	emit popup(show);
}


}//namespace cx

