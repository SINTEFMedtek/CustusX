/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxViewWrapper.h"

#include <QMenu>
#include "vtkCamera.h"

#include "cxViewGroup.h" //for class Navigation
#include "cxTypeConversions.h"
#include "cxDataMetric.h"
#include "cxView.h"
#include "cxImage.h"
#include "cxInteractiveClipper.h"
#include "cxNavigation.h"
#include "cxActiveData.h"
#include "cxSettings.h"
#include "cxDisplayTextRep.h"
#include "cxTrackingService.h"

namespace cx
{

DataViewPropertiesInteractor::DataViewPropertiesInteractor(VisServicesPtr services, ViewGroupDataPtr groupData) :
	mServices(services),
	mGroupData(groupData)
{
	mProperties = DataViewProperties::createDefault();
}

void DataViewPropertiesInteractor::setDataViewProperties(DataViewProperties properties)
{
	mProperties = properties;
}

void DataViewPropertiesInteractor::addDataAction(QString uid, QWidget* parent)
{
	DataPtr data = mServices->patient()->getData(uid);

	QAction* action = new QAction(qstring_cast(data->getName()), parent);

	action->setIcon(data->getIcon());

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
	DataPtr data = mServices->patient()->getData(uid);

	bool firstData = mGroupData->getData(DataViewProperties::createFull()).empty();

	DataViewProperties old = mGroupData->getProperties(uid);

	if (theAction->isChecked())
	{
		DataViewProperties props = old.addFlagsIn(mProperties);
		mGroupData->setProperties(uid, props);

		if (data)
		{
			ActiveDataPtr activeData = mServices->patient()->getActiveData();
			activeData->setActive(data);
		}
	}
	else
	{
		DataViewProperties props = old.removeFlagsIn(mProperties);
		mGroupData->setProperties(uid, props);
	}

	if (firstData)
	{
		Navigation(mServices).centerToDataInViewGroup(mGroupData);
		mGroupData->requestInitialize();
	}
}

///--------------------------------------------------------
///--------------------------------------------------------
///--------------------------------------------------------

ViewWrapper::ViewWrapper(VisServicesPtr services) :
	mServices(services)
{
}

void ViewWrapper::setViewGroup(ViewGroupDataPtr group)
{
	mGroupData = group;

	connect(mGroupData.get(), &ViewGroupData::dataViewPropertiesChanged, this, &ViewWrapper::dataViewPropertiesChangedSlot);
	connect(mGroupData.get(), &ViewGroupData::videoSourceChanged, this, &ViewWrapper::videoSourceChangedSlot);
	connect(mGroupData.get(), &ViewGroupData::controllingToolChanged, this, &ViewWrapper::activeToolChangedSlot);

	std::vector<DataPtr> data = mGroupData->getData();
	for (unsigned i = 0; i < data.size(); ++i)
		this->dataViewPropertiesChangedSlot(data[i]->getUid());


	mDataViewPropertiesInteractor.reset(new DataViewPropertiesInteractor(mServices, mGroupData));

	mShow3DSlicesInteractor.reset(new DataViewPropertiesInteractor(mServices, mGroupData));
	mShow3DSlicesInteractor->setDataViewProperties(DataViewProperties::createSlice3D());

	connect(settings(), SIGNAL(valueChangedFor(QString)), this, SLOT(settingsChangedSlot(QString)));
}

void ViewWrapper::settingsChangedSlot(QString key)
{
	if (key.startsWith("View"))
	{
		this->updateView();
	}
}

void ViewWrapper::contextMenuSlot(const QPoint& point)
{
	QMenu contextMenu;
	contextMenu.setToolTipsVisible(true);
	mDataViewPropertiesInteractor->addDataActionsOfType<Data>(&contextMenu);
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

void ViewWrapper::updateView()
{
	// view description
	QString annotationText;
	if (settings()->value("View/showOrientationAnnotation").value<bool>())
	{
		annotationText = QString("%1-%2")
				.arg(this->getViewDescription())
				.arg(mGroupData ? mGroupData->getUid() : "");
	}
	mPlaneTypeText->setText(0, annotationText);

	// data description
	QString showDataText;
	if (settings()->value("View/showDataText").value<bool>())
	{
		showDataText = this->getDataDescription();
	}
	mDataNameText->setText(0, showDataText);
	mDataNameText->setFontSize(std::max(12, 22 - 2 * showDataText.size()));
}

void ViewWrapper::addReps()
{
	// plane type text rep
	mPlaneTypeText = DisplayTextRep::New();
	mPlaneTypeText->addText(QColor(Qt::green), "--", Vector3D(0.98, 0.02, 0.0));
	this->getView()->addRep(mPlaneTypeText);

	//data name text rep
	mDataNameText = DisplayTextRep::New();
	mDataNameText->addText(QColor(Qt::green), "--", Vector3D(0.02, 0.02, 0.0));
	this->getView()->addRep(mDataNameText);
}

void ViewWrapper::setSharedOpenGLContext(cx::SharedOpenGLContextPtr sharedOpenGLContext)
{
	mSharedOpenGLContext = sharedOpenGLContext;
}

ToolPtr ViewWrapper::getControllingTool()
{
	ToolPtr activeTool = mServices->tracking()->getActiveTool();
	ToolPtr controllingTool;
	if (mGroupData)
		controllingTool = mGroupData->getControllingTool();
	
	if(!controllingTool)
		controllingTool = activeTool;
	
	return controllingTool;
}

} //namespace cx
