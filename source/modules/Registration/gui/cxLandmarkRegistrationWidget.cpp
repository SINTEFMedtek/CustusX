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

#include "cxLandmarkRegistrationWidget.h"

#include <sstream>
#include <QVBoxLayout>
#include <QPushButton>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QLabel>
#include <QSlider>
#include <vtkDoubleArray.h>
#include <vtkImageData.h>
#include "cxReporter.h"
#include "cxRegistrationManager.h"
#include "cxDataManager.h"
#include "cxRegistrationHistoryWidget.h"
#include "cxTypeConversions.h"
#include "cxManualTool.h"
#include "cxToolManager.h"

namespace cx
{
LandmarkRegistrationWidget::LandmarkRegistrationWidget(RegistrationServicePtr registrationService, QWidget* parent,
	QString objectName, QString windowTitle) :
	RegistrationBaseWidget(registrationService, parent, objectName, windowTitle), mVerticalLayout(new QVBoxLayout(this)),
		mLandmarkTableWidget(new QTableWidget(this)), mAvarageAccuracyLabel(new QLabel(QString(" "), this))
{
	//table widget
	connect(mLandmarkTableWidget, SIGNAL(cellClicked(int, int)), this, SLOT(cellClickedSlot(int, int)));
	connect(mLandmarkTableWidget, SIGNAL(cellChanged(int,int)), this, SLOT(cellChangedSlot(int,int)));

	this->setLayout(mVerticalLayout);
}

LandmarkRegistrationWidget::~LandmarkRegistrationWidget()
{
}

QString LandmarkRegistrationWidget::defaultWhatsThis() const
{
	return "<html>"
		"<h3>Registration.</h3>"
		"<p>Interface for registrating.</p>"
		"<p><i></i></p>"
		"</html>";
}

void LandmarkRegistrationWidget::cellClickedSlot(int row, int column)
{
	if (row < 0 || column < 0)
		return;

	if (!mLandmarkTableWidget)
		reporter()->sendDebug("mLandmarkTableWidget is null");

	mActiveLandmark = mLandmarkTableWidget->item(row, column)->data(Qt::UserRole).toString();


	LandmarkMap targetData = this->getTargetLandmarks();
	if (targetData.count(mActiveLandmark))
	{
		Vector3D p_d = targetData[mActiveLandmark].getCoord();
		Vector3D p_r = this->getTargetTransform().coord(p_d);
		Vector3D p_pr = dataManager()->get_rMpr().coord(p_r);
		this->setManualToolPosition(p_r);
	}

}

void LandmarkRegistrationWidget::setManualToolPosition(Vector3D p_r)
{
	Transform3D rMpr = dataManager()->get_rMpr();
	Vector3D p_pr = rMpr.inv().coord(p_r);

	// set the picked point as offset tip
	ManualToolPtr tool = toolManager()->getManualTool();
	Vector3D offset = tool->get_prMt().vector(Vector3D(0, 0, tool->getTooltipOffset()));
	p_pr -= offset;
	p_r = rMpr.coord(p_pr);

	// TODO set center here will not do: must handle
	dataManager()->setCenter(p_r);
	Vector3D p0_pr = tool->get_prMt().coord(Vector3D(0, 0, 0));
	tool->set_prMt(createTransformTranslate(p_pr - p0_pr) * tool->get_prMt());
}

void LandmarkRegistrationWidget::showEvent(QShowEvent* event)
{
	QWidget::showEvent(event);
	connect(dataManager(), SIGNAL(landmarkPropertiesChanged()), this, SLOT(landmarkUpdatedSlot()));

//	mManager->restart();
	mRegistrationService->setLastRegistrationTime(QDateTime::currentDateTime());
	this->setModified();
}

void LandmarkRegistrationWidget::hideEvent(QHideEvent* event)
{
	QWidget::hideEvent(event);
	disconnect(dataManager(), SIGNAL(landmarkPropertiesChanged()), this, SLOT(landmarkUpdatedSlot()));
}

void LandmarkRegistrationWidget::prePaintEvent()
{
	mLandmarkTableWidget->blockSignals(true);
	mLandmarkTableWidget->clear();

	QString fixedName;
	DataPtr fixedData = boost::dynamic_pointer_cast<Data>(mRegistrationService->getFixedData());
	if (fixedData)
		fixedName = fixedData->getName();

	std::vector<Landmark> landmarks = this->getAllLandmarks();
	LandmarkMap targetData = this->getTargetLandmarks();
	Transform3D rMtarget = this->getTargetTransform();

	//ready the table widget
	mLandmarkTableWidget->setRowCount((int)landmarks.size());
	mLandmarkTableWidget->setColumnCount(4);
	QStringList headerItems(QStringList() << "Name" << "Status" << "Coordinates" << "Accuracy (mm)");
	mLandmarkTableWidget->setHorizontalHeaderLabels(headerItems);
	mLandmarkTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
	mLandmarkTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);

	for (unsigned i = 0; i < landmarks.size(); ++i)
	{
		std::vector<QTableWidgetItem*> items(4); // name, status, coordinates, accuracy

		LandmarkProperty prop = dataManager()->getLandmarkProperties()[landmarks[i].getUid()];
		Vector3D coord = landmarks[i].getCoord();
		coord = rMtarget.coord(coord); // display coordinates in space r (in principle, this means all coords should be equal)

		items[0] = new QTableWidgetItem(qstring_cast(prop.getName()));
		items[0]->setToolTip(QString("Landmark name. Double-click to rename."));

		items[1] = new QTableWidgetItem;

		if (prop.getActive())
			items[1]->setCheckState(Qt::Checked);
		else
			items[1]->setCheckState(Qt::Unchecked);
		items[1]->setToolTip(QString("Check to use landmark in current registration."));

		QString coordText = "Not sampled";
		if (targetData.count(prop.getUid()))
		{
			int width = 5;
			int prec = 1;
			coordText = tr("(%1, %2, %3)").arg(coord[0], width, 'f', prec).arg(coord[1], width, 'f', prec).arg(
				coord[2], width, 'f', prec);
		}

		items[2] = new QTableWidgetItem(coordText);
		items[2]->setToolTip(QString("Landmark coordinates of target [%1] in reference space.").arg(this->getTargetName()));

		items[3] = new QTableWidgetItem(tr("%1").arg(this->getAccuracy(landmarks[i].getUid())));
		items[3]->setToolTip(QString("Distance from target [%1] to fixed [%2].").arg(this->getTargetName()).arg(fixedName));

		for (unsigned j = 0; j < items.size(); ++j)
		{
			items[j]->setData(Qt::UserRole, qstring_cast(prop.getUid()));
			mLandmarkTableWidget->setItem(i, j, items[j]);
		}

		//highlight selected row
		if (prop.getUid() == mActiveLandmark)
		{
			mLandmarkTableWidget->setCurrentItem(items[2]);
		}
	}

	this->updateAvarageAccuracyLabel();
	mLandmarkTableWidget->blockSignals(false);
}

void LandmarkRegistrationWidget::activateLandmark(QString uid)
{
    mActiveLandmark = uid;
    this->setModified();
}

/** Return the next landmark in the series of available landmarks,
  * beginning with the active landmark.
  */
QString LandmarkRegistrationWidget::getNextLandmark()
{
    std::vector<Landmark> lm = this->getAllLandmarks();

    for (int i=0; i<lm.size()-1; ++i)
    {
        if (lm[i].getUid()==mActiveLandmark)
        {
            return lm[i+1].getUid();
        }
    }

    return "";
}

std::vector<Landmark> LandmarkRegistrationWidget::getAllLandmarks() const
{
	std::vector<Landmark> retval;
	LandmarkMap targetData = this->getTargetLandmarks();
	std::map<QString, LandmarkProperty> dataData = dataManager()->getLandmarkProperties();
	std::map<QString, LandmarkProperty>::iterator iter;

	for (iter = dataData.begin(); iter != dataData.end(); ++iter)
	{
		if (targetData.count(iter->first))
			retval.push_back(targetData[iter->first]);
		else
			retval.push_back(Landmark(iter->first));
	}

	std::sort(retval.begin(), retval.end());

	return retval;
}

void LandmarkRegistrationWidget::cellChangedSlot(int row, int column)
{
	QTableWidgetItem* item = mLandmarkTableWidget->item(row, column);
	QString uid = item->data(Qt::UserRole).toString();

	if (column == 0)
	{
		QString name = item->text();
		dataManager()->setLandmarkName(uid, name);
	}
	if (column == 1)
	{
		Qt::CheckState state = item->checkState();
		dataManager()->setLandmarkActive(uid, state == Qt::Checked);
		this->performRegistration(); // automatic when changing active state (Mantis #0000674)s
	}
	if (column == 2)
	{
		QString val = item->text();
		// remove formatting stuff:
		val = val.replace('(', " ");
		val = val.replace(')', " ");
		val = val.replace(',', " ");

		Transform3D rMtarget = this->getTargetTransform();

		Vector3D p_r = Vector3D::fromString(val);
		Vector3D p_target = rMtarget.inv().coord(p_r);
		this->setTargetLandmark(uid, p_target);
	}
}

void LandmarkRegistrationWidget::landmarkUpdatedSlot()
{
//  - This has too many side effects when we use the landmarks for several different registrations,
//	i.e. image2image, patient, fast... Rather register explicitly, and add it to the buttons where you
//  want the automation, such as in the patient reg sampler. (Mantis #0000674)
//	this->performRegistration();
    this->setModified();
}

void LandmarkRegistrationWidget::updateAvarageAccuracyLabel()
{
	QString fixedName;
	DataPtr fixedData = boost::dynamic_pointer_cast<Data>(mRegistrationService->getFixedData());
	if (fixedData)
		fixedName = fixedData->getName();

	mAvarageAccuracyLabel->setText(tr("Mean accuracy %1 mm").arg(this->getAvarageAccuracy()));
	mAvarageAccuracyLabel->setToolTip(QString("Average landmark accuracy from target [%1] to fixed [%2].").arg(this->getTargetName()).arg(fixedName));
}

double LandmarkRegistrationWidget::getAvarageAccuracy()
{
	std::map<QString, LandmarkProperty> props = dataManager()->getLandmarkProperties();

	double sum = 0;
	int count = 0;
	std::map<QString, LandmarkProperty>::iterator it = props.begin();
	for (; it != props.end(); ++it)
	{
		if (!it->second.getActive()) //we don't want to take into account not active landmarks
			continue;
		QString uid = it->first;
		double val = this->getAccuracy(uid);
		if (!similar(val, 1000.0))
		{
			sum = sum + val;
			count++;
		}
	}
	if (count == 0)
		return 1000;
	return sum / count;
}

double LandmarkRegistrationWidget::getAccuracy(QString uid)
{
	DataPtr fixedData = mRegistrationService->getFixedData();
	if (!fixedData)
		return 1000.0;

	Landmark masterLandmark = fixedData->getLandmarks()->getLandmarks()[uid];
	Landmark targetLandmark = this->getTargetLandmarks()[uid];
	if (masterLandmark.getUid().isEmpty() || targetLandmark.getUid().isEmpty())
		return 1000.0;

	Vector3D p_master_master = masterLandmark.getCoord();
	Vector3D p_target_target = targetLandmark.getCoord();
	Transform3D rMmaster = fixedData->get_rMd();
	Transform3D rMtarget = this->getTargetTransform();

	Vector3D p_target_r = rMtarget.coord(p_target_target);
	Vector3D p_master_r = rMmaster.coord(p_master_master);

	return (p_target_r - p_master_r).length();
}

}//namespace cx
