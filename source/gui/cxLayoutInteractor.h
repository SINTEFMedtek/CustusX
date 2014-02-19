// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.

#ifndef CXLAYOUTINTERACTOR_H
#define CXLAYOUTINTERACTOR_H

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
 * \ingroup cxGUI
 * \date 2014-02-05
 * \author christiana
 */
class LayoutInteractor : public QObject
{
	Q_OBJECT
public:
	LayoutInteractor(QObject* parent = NULL);

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

	QActionGroup* mLayoutActionGroup; ///< grouping the view layout actions
	QAction* mNewLayoutAction; ///< create a new custom layout
	QAction* mEditLayoutAction; ///< edit the current custom layout
	QAction* mDeleteLayoutAction; ///< delete the current custom layout
	QPointer<QMenu> mMenu;
	QPointer<QMenu> mSecondaryLayoutMenu;
	QActionGroup* mSecondaryLayoutActionGroup;

};

typedef boost::shared_ptr<class LayoutInteractor> LayoutInteractorPtr;


} // namespace cx

#endif // CXLAYOUTINTERACTOR_H
