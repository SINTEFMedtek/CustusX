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

#include "cxViewWrapper.h"

#include <QMenu>
#include "vtkCamera.h"
#include "cxReporter.h"
#include "cxDataManager.h"
#include "cxViewGroup.h" //for class Navigation
#include "cxMesh.h"
#include "cxTypeConversions.h"
#include "cxImageAlgorithms.h"
#include "cxDataMetric.h"
#include "cxView.h"
#include "cxImage.h"
#include "cxViewManager.h"
#include "cxInteractiveClipper.h"
#include "cxRepManager.h"
#include "cxVisualizationServiceBackend.h"
#include "cxNavigation.h"

namespace cx
{

SyncedValue::SyncedValue(QVariant val) :
				mValue(val)
{
}
SyncedValuePtr SyncedValue::create(QVariant val)
{
	return SyncedValuePtr(new SyncedValue(val));
}
void SyncedValue::set(QVariant val)
{
	if (mValue == val)
		return;
	mValue = val;
	emit changed();
}
QVariant SyncedValue::get() const
{
	return mValue;
}

///--------------------------------------------------------
///--------------------------------------------------------
///--------------------------------------------------------

DataViewPropertiesInteractor::DataViewPropertiesInteractor(VisualizationServiceBackendPtr backend, ViewGroupDataPtr groupData) :
	mBackend(backend),
	mGroupData(groupData)
{
	mProperties = DataViewProperties::createDefault();
}

void DataViewPropertiesInteractor::addDataActions(QWidget* parent)
{
	//add actions to the actiongroups and the contextmenu
	std::vector<DataPtr> sorted = sortOnGroupsAndAcquisitionTime(mBackend->getDataManager()->getData());
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
	DataPtr data = mBackend->getDataManager()->getData(uid);

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
	DataPtr data = mBackend->getDataManager()->getData(uid);
	ImagePtr image = mBackend->getDataManager()->getImage(data->getUid());

	bool firstData = mGroupData->getData(DataViewProperties::createFull()).empty();

	DataViewProperties old = mGroupData->getProperties(data);

	if (theAction->isChecked())
	{
		DataViewProperties props = old.addFlagsIn(mProperties);
		mGroupData->setProperties(data, props);

		if (image)
			mBackend->getDataManager()->setActiveImage(image);
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

ViewWrapper::ViewWrapper(VisualizationServiceBackendPtr backend) :
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

//void ViewWrapper::dataAddedSlot(QString uid)
//{
//	this->dataAdded(mBackend->getDataManager()->getData(uid));
//}

//void ViewWrapper::dataRemovedSlot(QString uid)
//{
//	this->dataRemoved(uid);
//}

void ViewWrapper::contextMenuSlot(const QPoint& point)
{
	QWidget* sender = dynamic_cast<QWidget*>(this->sender());
	QPoint pointGlobal = sender->mapToGlobal(point);
	QMenu contextMenu(sender);

	mDataViewPropertiesInteractor->addDataActions(&contextMenu);

	QMenu* show3DSlicesMenu = new QMenu("Show 3D slices");
	contextMenu.addMenu(show3DSlicesMenu);
	mShow3DSlicesInteractor->addDataActions(show3DSlicesMenu);

	//append specific info from derived classes
	this->appendToContextMenu(contextMenu);

	contextMenu.exec(pointGlobal);
}


void ViewWrapper::connectContextMenu(ViewWidget* view)
{
	connect(view, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(contextMenuSlot(const QPoint &)));
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
