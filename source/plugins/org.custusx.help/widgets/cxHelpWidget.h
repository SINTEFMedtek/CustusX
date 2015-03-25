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
#ifndef CXHELPWIDGET_H
#define CXHELPWIDGET_H

#include "cxBaseWidget.h"
#include "boost/shared_ptr.hpp"
#include "org_custusx_help_Export.h"
class QTabWidget;
class QAction;
class QSplitter;

namespace cx
{
typedef boost::shared_ptr<class HelpEngine> HelpEnginePtr;
class HelpSearchWidget;
class HelpIndexWidget;

/**
 * Top-level help widget
 *
 * \ingroup org_custusx_help
 *
 * \date 2014-09-30
 * \author Christian Askeland
 */
class org_custusx_help_EXPORT HelpWidget : public BaseWidget
{
	Q_OBJECT

public:
	explicit HelpWidget(HelpEnginePtr engine, QWidget* parent = NULL);
	virtual ~HelpWidget();

	virtual QString defaultWhatsThis() const;
	HelpEnginePtr engine() { return mEngine; }

signals:
	void requestShowLink(const QUrl&);

private slots:
	void toggleShowNavigationControls();
	void backSlot();
	void forwardSlot();
private:
	virtual void showEvent(QShowEvent* event); ///<updates internal info before showing the widget
	virtual void hideEvent(QHideEvent* event);
	virtual void prePaintEvent();
	void setup();
	void addSearchWidget(QTabWidget* tabWidget, QBoxLayout* buttonLayout);
	void addIndexWidget(QTabWidget* tabWidget, QBoxLayout* buttonLayout);
	void addContentWidget(QTabWidget* tabWidget, QBoxLayout* buttonLayout);
	void addToggleTabWidgetButton(QBoxLayout* buttonLayout);
	void addWebNavigationButtons(QBoxLayout* buttonLayout);

	QVBoxLayout* mVerticalLayout;
	QTabWidget* mTabWidget;
	HelpEnginePtr mEngine;

	HelpSearchWidget* mSearchWidget;
	HelpIndexWidget* mIndexWidget;
	class HelpBrowser *mBrowser ;

	QAction* mShowNavigationControlsAction;
	QSplitter* mSplitter;

};

}//end namespace cx

#endif // CXHELPWIDGET_H
