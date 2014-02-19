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

#include "cxCameraStyle.h"

#include <QWidget>
#include <QMenu>
#include <QAction>
#include <QContextMenuEvent>
#include <vtkRenderer.h>
#include <vtkCamera.h>
#include "sscImage.h"
#include "sscVolumetricRep.h"
#include "sscMessageManager.h"
#include "cxRepManager.h"
#include "sscToolManager.h"
#include "sscToolRep3D.h"
#include "sscTypeConversions.h"
#include "cxViewManager.h"
#include "sscView.h"
#include "sscTool.h"
#include "boost/bind.hpp"
#include <vtkRenderWindow.h>
#include "vtkRenderWindowInteractor.h"
#include "vtkInteractorStyleUnicam.h"
#include "vtkInteractorStyleTrackballCamera.h"
#include "vtkInteractorStyleTrackballActor.h"
#include "vtkInteractorStyleFlight.h"

#include "cxViewGroup.h"

namespace cx
{

CameraStyle::CameraStyle() :
	mCameraStyle(cstDEFAULT_STYLE),
    mCameraStyleGroup(NULL)
{
	connect(viewManager(), SIGNAL(activeLayoutChanged()), this, SLOT(viewChangedSlot()));
	this->viewChangedSlot();
}

/**reset the view connection, this is in case the view, reps or tool has been deleted/recreated in
 * the layout change process.
 */
void CameraStyle::viewChangedSlot()
{
	mViews.clear();

	std::vector<ViewWidgetQPtr> views = viewManager()->getViewGroups()[0]->getViews();
	for (unsigned i = 0; i < views.size(); ++i)
	{
		if(!views[i])
			continue;
		if (views[i]->getType()!=View::VIEW_3D)
			continue;
		CameraStyleForViewPtr style(new CameraStyleForView());
		style->setView(views[i]);
		mViews.push_back(style);
	}
}

QActionGroup* CameraStyle::createInteractorStyleActionGroup()
{
	if (mCameraStyleGroup)
		return mCameraStyleGroup;

	mCameraStyleGroup = new QActionGroup(this);
	mCameraStyleGroup->setExclusive(true);

	this->addInteractorStyleAction("Normal Camera", mCameraStyleGroup,
	                               enum2string(cstDEFAULT_STYLE),
	                               QIcon(":/icons/camera-n.png"),
	                               "Set 3D interaction to the normal camera-oriented style.");
	this->addInteractorStyleAction("Tool", mCameraStyleGroup,
	                               enum2string(cstTOOL_STYLE),
	                               QIcon(":/icons/camera-t.png"),
	                               "Camera following tool.");
	this->addInteractorStyleAction("Angled Tool", mCameraStyleGroup,
	                               enum2string(cstANGLED_TOOL_STYLE),
	                               QIcon(":/icons/camera-at.png"),
	                               "Camera following tool (Placed at an angle of 20 degrees).");
	this->addInteractorStyleAction("Unicam", mCameraStyleGroup,
	                               enum2string(cstUNICAM_STYLE),
	                               QIcon(":/icons/camera-u.png"),
	                               "Set 3D interaction to a single-button style, useful for touch screens.");

//	this->addInteractorStyleAction("Object", camGroup, "vtkInteractorStyleTrackballActor",
//					QIcon(":/icons/camera-o.png"), "Set 3D interaction to a object-oriented style.");
//	this->addInteractorStyleAction("Flight", camGroup, "vtkInteractorStyleFlight", QIcon(":/icons/camera-f.png"),
//					"Set 3D interaction to a flight style.");

	return mCameraStyleGroup;
}

void CameraStyle::addInteractorStyleAction(QString caption, QActionGroup* group, QString uid, QIcon icon,
				QString helptext)
{
//	vtkRenderWindowInteractor* interactor = NULL;
//	ViewWidget* view = this->getView();
//	if (view)
//		interactor = view->getRenderWindow()->GetInteractor();

	QAction* action = new QAction(caption, group);
	action->setIcon(icon);
	action->setCheckable(true);
	action->setData(uid);
	action->setToolTip(helptext);
	action->setWhatsThis(helptext);
//	if (interactor)
		action->setChecked(enum2string(mCameraStyle) == uid);
	connect(action, SIGNAL(triggered(bool)), this, SLOT(setInteractionStyleActionSlot()));
}

void CameraStyle::updateActionGroup()
{
	QList<QAction*> actions = mCameraStyleGroup->actions();
	for (int i=0; i<actions.size(); ++i)
	{
		actions[i]->blockSignals(true);
		actions[i]->setChecked(actions[i]->data().toString() == enum2string(mCameraStyle));
		actions[i]->blockSignals(false);
	}
}

void CameraStyle::setInteractionStyleActionSlot()
{
	QAction* theAction = static_cast<QAction*>(sender());
	if(!theAction)
		return;

	QString uid = theAction->data().toString();
	CAMERA_STYLE_TYPE newStyle = string2enum<cx::CAMERA_STYLE_TYPE>(uid);
	if (newStyle==cstCOUNT)
		return;

	this->setCameraStyle(newStyle);
}

void CameraStyle::setCameraStyle(CAMERA_STYLE_TYPE style)
{
	if (mCameraStyle == style)
		return;

	for (unsigned i=0; i<mViews.size(); ++i)
	{
		mViews[i]->setCameraStyle(style);
	}

	mCameraStyle = style;

	this->updateActionGroup();
	messageManager()->sendInfo(QString("Activated camera style %1.").arg(enum2string(style)));
}

}//namespace cx
