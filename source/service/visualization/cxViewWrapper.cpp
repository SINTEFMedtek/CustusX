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
#include "sscMessageManager.h"
#include "sscDataManager.h"
#include "cxViewGroup.h" //for class Navigation
#include "sscMesh.h"
#include "sscTypeConversions.h"
#include "cxCameraControl.h"
#include "sscImageAlgorithms.h"
#include "sscDataMetric.h"
#include "sscView.h"
#include "sscImage.h"
#include "cxViewManager.h"
#include "cxInteractiveClipper.h"
#include "cxRepManager.h"

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


void ViewWrapper::setViewGroup(ViewGroupDataPtr group)
{
	mGroupData = group;
	connect(mGroupData.get(), SIGNAL(dataAdded(QString)), SLOT(dataAddedSlot(QString)));
	connect(mGroupData.get(), SIGNAL(dataRemoved(QString)), SLOT(dataRemovedSlot(QString)));
	connect(mGroupData.get(), SIGNAL(videoSourceChanged(QString)), SLOT(videoSourceChangedSlot(QString)));

	std::vector<DataPtr> data = mGroupData->getData();
	for (unsigned i = 0; i < data.size(); ++i)
	{
		this->dataAddedSlot(qstring_cast(data[i]->getUid()));
	}

}

void ViewWrapper::dataAddedSlot(QString uid)
{
	this->dataAdded(dataManager()->getData(uid));
}

void ViewWrapper::dataRemovedSlot(QString uid)
{
	this->dataRemoved(uid);
//	RepManager::getInstance()->purgeVolumetricReps();
}

void ViewWrapper::contextMenuSlot(const QPoint& point)
{
	QWidget* sender = dynamic_cast<QWidget*>(this->sender());QPoint pointGlobal = sender->mapToGlobal(point);
	QMenu contextMenu(sender);

	//add actions to the actiongroups and the contextmenu
	std::vector<DataPtr> sorted = sortOnGroupsAndAcquisitionTime(dataManager()->getData());
	mLastDataActionUid = "________________________";
	for (std::vector<DataPtr>::iterator iter=sorted.begin(); iter!=sorted.end(); ++iter)
	{
		this->addDataAction((*iter)->getUid(), &contextMenu);
	}

	//append specific info from derived classes
	this->appendToContextMenu(contextMenu);

	contextMenu.exec(pointGlobal);
}

void ViewWrapper::addDataAction(QString uid, QMenu* contextMenu)
{
	DataPtr data = dataManager()->getData(uid);

	QAction* action = new QAction(qstring_cast(data->getName()), contextMenu);

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
	std::vector<DataPtr> allVisible = mGroupData->getData();
	action->setChecked(std::count(allVisible.begin(), allVisible.end(), data));
	connect(action, SIGNAL(triggered()), this, SLOT(dataActionSlot()));
	contextMenu->addAction(action);
}

void ViewWrapper::dataActionSlot()
{
	QAction* theAction = static_cast<QAction*>(sender());if(!theAction)
	return;

	QString uid = theAction->data().toString();
	DataPtr data = dataManager()->getData(uid);
	ImagePtr image = dataManager()->getImage(data->getUid());

	bool firstData = mGroupData->getData().empty();

	if (theAction->isChecked())
	{
		mGroupData->addData(data);
		if (image)
		dataManager()->setActiveImage(image);
	}
	else
	{
		mGroupData->removeData(data);
		//if (image)
		//dataManager()->setActiveImage(ImagePtr());
	}

	if (firstData)
	{
		Navigation().centerToGlobalDataCenter(); // reset center for convenience
					mGroupData->requestInitialize();
				}
			}

void ViewWrapper::connectContextMenu(ViewWidget* view)
{
	connect(view, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(contextMenuSlot(const QPoint &)));
}

QStringList ViewWrapper::getAllDataNames() const
{
	if (!mGroupData)
		return QStringList();
	std::vector<DataPtr> data = mGroupData->getData();

	QStringList text;
	for (unsigned i = 0; i < data.size(); ++i)
	{
		QString line = data[i]->getName();

		ImagePtr image = boost::dynamic_pointer_cast<Image>(data[i]);
		if (image)
		{
			if (image->getCropping())
				line += " (cropped)";
			if (!image->getAllClipPlanes().empty() || ((viewManager()->getClipper()->getImage() == image) &&  viewManager()->getClipper()->getUseClipper()))
				line += " (clipped)";
		}

		text << line;
	}
	std::reverse(text.begin(), text.end());
	return text;
}

} //namespace cx
