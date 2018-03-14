/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxCameraStyleInteractor.h"


#include <QAction>
#include "cxLogger.h"
#include "cxView.h"
#include "cxLogger.h"
#include "cxViewGroupData.h"

namespace cx
{

CameraStyleInteractor::CameraStyleInteractor() :
	mCameraStyleGroup(NULL)
{
}

void CameraStyleInteractor::connectCameraStyle(ViewGroupDataPtr vg)
{
	if (mGroup)
		disconnect(mGroup.get(), &ViewGroupData::optionsChanged, this, &CameraStyleInteractor::updateActionGroup);
	mGroup = vg;
	if (mGroup)
		connect(mGroup.get(), &ViewGroupData::optionsChanged, this, &CameraStyleInteractor::updateActionGroup);
	this->updateActionGroup();
}

QActionGroup* CameraStyleInteractor::getInteractorStyleActionGroup()
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
	if (mGroup)
		currentStyle = enum2string(mGroup->getOptions().mCameraStyle.getStyle());

	if(mCameraStyleGroup)
	{
		QList<QAction*> actions = mCameraStyleGroup->actions();
		for (int i=0; i<actions.size(); ++i)
		{
			actions[i]->setEnabled(mGroup!=0);
			bool check = actions[i]->data().toString() == currentStyle;
			if(actions[i]->isChecked() != check)
				actions[i]->setChecked(check);
		}
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

	if (mGroup)
	{
		ViewGroupData::Options options = mGroup->getOptions();
		options.mCameraStyle = CameraStyleData(newStyle);
		mGroup->setOptions(options);
	}
}



}//namespace cx
