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

#include <QAction>
#include "cxReporter.h"
#include "cxView.h"
#include "cxViewGroup.h"

namespace cx
{

CameraStyle::CameraStyle(VisualizationServiceBackendPtr backend) :
	mCameraStyle(cstDEFAULT_STYLE),
	mBackend(backend)
{
}

void CameraStyle::addView(ViewWidgetQPtr view)
{
	if (!view || view->getType()!=View::VIEW_3D)
		return;

	CameraStyleForViewPtr style(new CameraStyleForView(mBackend));
	style->setView(view);
	style->setCameraStyle(mCameraStyle);
	mViews.push_back(style);
}

void CameraStyle::clearViews()
{
	mViews.clear();
}

CAMERA_STYLE_TYPE CameraStyle::getCameraStyle() const
{
	return mCameraStyle;
}

void CameraStyle::setCameraStyle(CAMERA_STYLE_TYPE style)
{
	if (mCameraStyle == style)
		return;

	for (unsigned i=0; i<mViews.size(); ++i)
		mViews[i]->setCameraStyle(style);
	mCameraStyle = style;

	emit cameraStyleChanged();
	report(QString("Activated camera style %1.").arg(enum2string(style)));
}

///--------------------------------------------------------
///--------------------------------------------------------
///--------------------------------------------------------

CameraStyleInteractor::CameraStyleInteractor() :
	mCameraStyleGroup(NULL)
{
}

void CameraStyleInteractor::connectCameraStyle(CameraStylePtr style)
{
	if (mStyle)
		disconnect(mStyle.get(), SIGNAL(cameraStyleChanged()), this, SLOT(updateActionGroup()));
	mStyle = style;
	if (mStyle)
		connect(mStyle.get(), SIGNAL(cameraStyleChanged()), this, SLOT(updateActionGroup()));
	this->updateActionGroup();
}

QActionGroup* CameraStyleInteractor::createInteractorStyleActionGroup()
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

	return mCameraStyleGroup;
}

void CameraStyleInteractor::addInteractorStyleAction(QString caption, QActionGroup* group, QString uid, QIcon icon,
				QString helptext)
{
	QAction* action = new QAction(caption, group);
	action->setIcon(icon);
	action->setCheckable(true);
	action->setData(uid);
	action->setToolTip(helptext);
	action->setWhatsThis(helptext);
	connect(action, SIGNAL(triggered(bool)), this, SLOT(setInteractionStyleActionSlot()));
}

void CameraStyleInteractor::updateActionGroup()
{
	QString currentStyle;
	if (mStyle)
		currentStyle = enum2string(mStyle->getCameraStyle());

	QList<QAction*> actions = mCameraStyleGroup->actions();
	for (int i=0; i<actions.size(); ++i)
	{
		actions[i]->blockSignals(true);
		actions[i]->setChecked(actions[i]->data().toString() == currentStyle);
		actions[i]->setEnabled(mStyle!=0);
		actions[i]->blockSignals(false);
	}

}

void CameraStyleInteractor::setInteractionStyleActionSlot()
{
	QAction* theAction = static_cast<QAction*>(sender());
	if(!theAction)
		return;

	QString uid = theAction->data().toString();
	CAMERA_STYLE_TYPE newStyle = string2enum<cx::CAMERA_STYLE_TYPE>(uid);
	if (newStyle==cstCOUNT)
		return;

	if (mStyle)
		mStyle->setCameraStyle(newStyle);
}



}//namespace cx
