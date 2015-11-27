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
#include "cxPopupToolbarWidget.h"

#include <QHBoxLayout>
#include <QToolButton>
#include <QIcon>
#include "cxBaseWidget.h"

namespace cx
{


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

PopupToolbarWidget::PopupToolbarWidget(QWidget* parent) :
	QWidget(parent)
{
	mControlLayout = new QHBoxLayout(this);
	mControlLayout->setMargin(0);
//	layout->addLayout(mControlLayout);

	mShowControlsButton = new PopupButton(this->parentWidget());
	mControlLayout->addWidget(mShowControlsButton);
	connect(mShowControlsButton, &PopupButton::popup, this, &PopupToolbarWidget::onPopup);

	mButtonWidget = new QWidget(this);
//	mButtonWidget->setVisible(false);
	mControlLayout->addWidget(mButtonWidget);

//	mControlLayout->addStretch(1);

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

void PopupToolbarWidget::onPopup()
{
	std::cout << "PopupToolbarWidget::onPopup()" << std::endl;
	bool show = mShowControlsButton->getShowPopup();

//	mMessagesWidget->showHeader(show);
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

