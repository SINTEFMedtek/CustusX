/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cx2DZoomHandler.h"
#include "cxViewGroup.h"
#include "cxLogger.h"
#include "cxUtilHelpers.h"
#include "cxSyncedValue.h"

namespace cx
{

Zoom2DHandler::Zoom2DHandler()
{
	this->set(SyncedValue::create(1.0));
}

void Zoom2DHandler::setGroupData(ViewGroupDataPtr group)
{
	mGroupData = group;
	this->set(group->getGlobal2DZoom());
}

double Zoom2DHandler::getFactor()
{
	return mZoom2D->get().toDouble();
}

void Zoom2DHandler::setFactor(double factor)
{
	factor = constrainValue(factor, 0.2, 10.0);
	mZoom2D->set(factor);
}

void Zoom2DHandler::set(SyncedValuePtr value)
{
	if (mZoom2D)
		disconnect(mZoom2D.get(), SIGNAL(changed()), this, SIGNAL(zoomChanged()));
	mZoom2D = value;
	if (mZoom2D)
		connect(mZoom2D.get(), SIGNAL(changed()), this, SIGNAL(zoomChanged()));

	emit zoomChanged();
}

void Zoom2DHandler::addActionsToMenu(QMenu* contextMenu)
{
	this->addConnectivityAction("global", "Global 2D Zoom", contextMenu);
	this->addConnectivityAction("group", "Group 2D Zoom", contextMenu);
	this->addConnectivityAction("local", "Disconnected 2D Zoom", contextMenu);
	contextMenu->addSeparator();
}

void Zoom2DHandler::addConnectivityAction(QString type, QString text, QMenu* contextMenu)
{
	QAction* action = new QAction(text, contextMenu);
	action->setCheckable(true);
	action->setData(type);
	action->setChecked(this->getConnectivityType()==type);
	connect(action, SIGNAL(triggered()), this, SLOT(zoom2DActionSlot()));
	contextMenu->addAction(action);
}

/** Slot for the global zoom action
	 *  Set the global zoom flag in the view manager.
	 */
void Zoom2DHandler::zoom2DActionSlot()
{
	QAction* theAction = static_cast<QAction*>(sender());
	if(!theAction)
		return;

	QString action = theAction->data().toString();
	this->setConnectivityFromType(action);
}

QString Zoom2DHandler::getConnectivityType()
{
	if (!mGroupData)
		return "local";
	if (mGroupData->getGroup2DZoom() == mZoom2D)
		return "group";
	if (mGroupData->getGlobal2DZoom() == mZoom2D)
		return "global";
	return "local";
}

void Zoom2DHandler::setConnectivityFromType(QString type)
{
	if (!mGroupData)
		type = "local";

	if (type=="global")
	{
		this->set(mGroupData->getGlobal2DZoom());
	}
	else if (type=="group")
	{
		this->set(mGroupData->getGroup2DZoom());
	}
	else if (type=="local")
	{
		this->set(SyncedValue::create(this->getFactor()));
	}
	else
	{
		reportWarning(QString("No zoom connectivity found for type [%1].").arg(type));
	}
}




} // namespace cx


