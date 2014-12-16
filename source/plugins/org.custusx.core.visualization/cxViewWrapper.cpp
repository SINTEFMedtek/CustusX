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

#include "cxViewWrapper.h"

#include <QMenu>
#include "vtkCamera.h"

#include "cxPatientModelService.h"
#include "cxViewGroup.h" //for class Navigation
#include "cxMesh.h"
#include "cxTypeConversions.h"
#include "cxImageAlgorithms.h"
#include "cxDataMetric.h"
#include "cxView.h"
#include "cxImage.h"
#include "cxViewManager.h"
#include "cxInteractiveClipper.h"
#include "cxCoreServices.h"
#include "cxNavigation.h"

namespace cx
{

DataViewPropertiesInteractor::DataViewPropertiesInteractor(CoreServicesPtr backend, ViewGroupDataPtr groupData) :
	mBackend(backend),
	mGroupData(groupData)
{
	mProperties = DataViewProperties::createDefault();
}

void DataViewPropertiesInteractor::addDataActions(QWidget* parent)
{
	//add actions to the actiongroups and the contextmenu
	std::vector<DataPtr> sorted = sortOnGroupsAndAcquisitionTime(mBackend->getPatientService()->getData());
	mLastDataActionUid = "________________________";
	for (std::vector<DataPtr>::iterator iter=sorted.begin(); iter!=sorted.end(); ++iter)
	{
		this->addDataAction((*iter)->getUid(), parent);
	}
}

void DataViewPropertiesInteractor::setDataViewProperties(DataViewProperties properties)
{
	mProperties = properties;
}

void DataViewPropertiesInteractor::addDataAction(QString uid, QWidget* parent)
{
	DataPtr data = mBackend->getPatientService()->getData(uid);

	QAction* action = new QAction(qstring_cast(data->getName()), parent);

	if (boost::dynamic_pointer_cast<Image>(data))
		action->setIcon(QIcon(":/icons/volume.png"));
	else if (boost::dynamic_pointer_cast<Mesh>(data))
		action->setIcon(QIcon(":/icons/surface.png"));
	else if (boost::dynamic_pointer_cast<DataMetric>(data))
		action->setIcon(QIcon(":/icons/metric.png"));

//  std::cout << "base " << mLastDataActionUid << "  " << uid << std::endl;
	if (uid.contains(mLastDataActionUid))
	{
		action->setText("    " + action->text());
//    std::cout << "indenting " << action->text() << std::endl;
	}
	else
	{
		mLastDataActionUid = uid;
	}

	action->setData(QVariant(qstring_cast(uid)));
	action->setCheckable(true);
	std::vector<DataPtr> allVisible = mGroupData->getData(mProperties);
	action->setChecked(std::count(allVisible.begin(), allVisible.end(), data));
	connect(action, SIGNAL(triggered()), this, SLOT(dataActionSlot()));
	parent->addAction(action);
}

void DataViewPropertiesInteractor::dataActionSlot()
{
	QAction* theAction = static_cast<QAction*>(sender());
	if(!theAction)
		return;

	QString uid = theAction->data().toString();
	DataPtr data = mBackend->getPatientService()->getData(uid);
	ImagePtr image = mBackend->getPatientService()->getData<Image>(data->getUid());

	bool firstData = mGroupData->getData(DataViewProperties::createFull()).empty();

	DataViewProperties old = mGroupData->getProperties(data);

	if (theAction->isChecked())
	{
		DataViewProperties props = old.addFlagsIn(mProperties);
		mGroupData->setProperties(data, props);

		if (image)
			mBackend->getPatientService()->setActiveImage(image);
	}
	else
	{
		DataViewProperties props = old.removeFlagsIn(mProperties);
		mGroupData->setProperties(data, props);
	}

	if (firstData)
	{
		Navigation(mBackend).centerToGlobalDataCenter(); // reset center for convenience
		mGroupData->requestInitialize();
	}
}

///--------------------------------------------------------
///--------------------------------------------------------
///--------------------------------------------------------

ViewWrapper::ViewWrapper(CoreServicesPtr backend) :
	mBackend(backend)
{
}

void ViewWrapper::setViewGroup(ViewGroupDataPtr group)
{
	mGroupData = group;

	connect(mGroupData.get(), SIGNAL(dataViewPropertiesChanged(QString)), SLOT(dataViewPropertiesChangedSlot(QString)));
//	connect(mGroupData.get(), SIGNAL(dataAdded(QString)), SLOT(dataAddedSlot(QString)));
//	connect(mGroupData.get(), SIGNAL(dataRemoved(QString)), SLOT(dataRemovedSlot(QString)));
	connect(mGroupData.get(), SIGNAL(videoSourceChanged(QString)), SLOT(videoSourceChangedSlot(QString)));

	std::vector<DataPtr> data = mGroupData->getData();
	for (unsigned i = 0; i < data.size(); ++i)
		this->dataViewPropertiesChangedSlot(data[i]->getUid());

	mDataViewPropertiesInteractor.reset(new DataViewPropertiesInteractor(mBackend, mGroupData));

	mShow3DSlicesInteractor.reset(new DataViewPropertiesInteractor(mBackend, mGroupData));
	mShow3DSlicesInteractor->setDataViewProperties(DataViewProperties::createSlice3D());
}

void ViewWrapper::dataViewPropertiesChangedSlot(QString uid)
{
}

void ViewWrapper::contextMenuSlot(const QPoint& point)
{
	QMenu contextMenu;
	mDataViewPropertiesInteractor->addDataActions(&contextMenu);
	//append specific info from derived classes
	this->appendToContextMenu(contextMenu);
	contextMenu.exec(point);
}


void ViewWrapper::connectContextMenu(ViewPtr view)
{
	connect(view.get(), SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(contextMenuSlot(const QPoint &)));
}

QStringList ViewWrapper::getAllDataNames(DataViewProperties properties) const
{
	if (!mGroupData)
		return QStringList();
	std::vector<DataPtr> data = mGroupData->getData(properties);

	QStringList text;
	for (unsigned i = 0; i < data.size(); ++i)
	{
		DataMetricPtr metric = boost::dynamic_pointer_cast<DataMetric>(data[i]);
		if (metric) // dont show metrics here: too much spam - use separate list is necessary
			continue;
		QString line = data[i]->getName();

		ImagePtr image = boost::dynamic_pointer_cast<Image>(data[i]);
		if (image)
		{
			if (image->getCropping())
				line += " (cropped)";
			if (!image->getAllClipPlanes().empty())
				line += " (clipped)";
		}

		text << line;
	}
	std::reverse(text.begin(), text.end());
	return text;
}

} //namespace cx
