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

#ifndef CXLAYOUTINTERACTOR_H
#define CXLAYOUTINTERACTOR_H

#include "cxGuiExport.h"

#include <QObject>
#include <QAction>
#include <QActionGroup>
#include <cxForwardDeclarations.h>
#include "cxLayoutData.h"


namespace cx
{
typedef boost::shared_ptr<class LayoutRepository> LayoutRepositoryPtr;

/** 
 *
 *
 * \ingroup cx_gui
 * \date 2014-02-05
 * \author christiana
 */
class cxGui_EXPORT LayoutInteractor : public QObject
{
	Q_OBJECT
public:
	LayoutInteractor(ViewServicePtr viewService, QObject* parent = NULL);

	void connectToMenu(QMenu* menu);

private slots:
	// layout menu
	void layoutChangedSlot();
	void newCustomLayoutSlot();
	void editCustomLayoutSlot();
	void deleteCustomLayoutSlot();

private:
	void createActions();
	LayoutData executeLayoutEditorDialog(QString title, bool createNew);
	QActionGroup* createLayoutActionGroup(int widgetIndex);
	QAction* addLayoutAction(QString layout, QActionGroup* group, int widgetIndex);
	void setActiveLayout(QString layout, int widgetIndex);
	void deepDeleteActionGroup(QActionGroup* actionGroup);
	LayoutRepositoryPtr getRepo();

	QActionGroup* mLayoutActionGroup; ///< grouping the view layout actions
	QAction* mNewLayoutAction; ///< create a new custom layout
	QAction* mEditLayoutAction; ///< edit the current custom layout
	QAction* mDeleteLayoutAction; ///< delete the current custom layout
	QPointer<QMenu> mMenu;
	QPointer<QMenu> mSecondaryLayoutMenu;
	QActionGroup* mSecondaryLayoutActionGroup;
	ViewServicePtr mViewService;


};

typedef boost::shared_ptr<class LayoutInteractor> LayoutInteractorPtr;


} // namespace cx

#endif // CXLAYOUTINTERACTOR_H
