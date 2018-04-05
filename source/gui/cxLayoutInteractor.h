/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
