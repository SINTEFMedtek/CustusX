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


