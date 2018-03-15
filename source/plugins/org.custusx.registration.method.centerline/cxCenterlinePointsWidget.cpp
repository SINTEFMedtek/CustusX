/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxCenterlinePointsWidget.h"

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
#include "cxLogger.h"
#include "cxPickerRep.h"
#include "cxLabeledComboBoxWidget.h"
#include "cxSettings.h"
#include "cxLandmarkRep.h"
#include "cxView.h"
#include "cxTypeConversions.h"
#include "cxSelectDataStringProperty.h"
#include "cxRegistrationService.h"
#include "cxPatientModelService.h"
#include "cxViewService.h"
#include "cxRepContainer.h"
#include "cxTrackingService.h"
#include "cxLandmarkListener.h"
#include "cxActiveData.h"
#include "cxMesh.h"
#include "cxTime.h"

namespace cx
{
CenterlinePointsWidget::CenterlinePointsWidget(RegServicesPtr services, QWidget* parent,
	QString objectName, QString windowTitle, bool useRegistrationFixedPropertyInsteadOfActiveImage) :
    LandmarkRegistrationWidget(services, parent, objectName, windowTitle, false),
	mUseRegistrationFixedPropertyInsteadOfActiveImage(useRegistrationFixedPropertyInsteadOfActiveImage)
{
	if(mUseRegistrationFixedPropertyInsteadOfActiveImage)
		mCurrentProperty.reset(new StringPropertyRegistrationFixedImage(services->registration(), services->patient()));
	else
		mCurrentProperty = StringPropertySelectData::New(mServices->patient());
    connect(mCurrentProperty.get(), &Property::changed, this, &CenterlinePointsWidget::onCurrentImageChanged);

	mLandmarkListener->useOnlyOneSourceUpdatedFromOutside();

	mActiveToolProxy = ActiveToolProxy::New(services->tracking());
	connect(mActiveToolProxy.get(), SIGNAL(toolVisible(bool)), this, SLOT(enableButtons()));
	connect(mActiveToolProxy.get(), SIGNAL(activeToolChanged(const QString&)), this, SLOT(enableButtons()));

	//pushbuttons
	mAddLandmarkButton = new QPushButton("New Landmark", this);
	mAddLandmarkButton->setToolTip("Add landmark");
	mAddLandmarkButton->setDisabled(true);
	connect(mAddLandmarkButton, SIGNAL(clicked()), this, SLOT(addLandmarkButtonClickedSlot()));

	mEditLandmarkButton = new QPushButton("Resample", this);
	mEditLandmarkButton->setToolTip("Resample existing landmark");
	mEditLandmarkButton->setDisabled(true);
	connect(mEditLandmarkButton, SIGNAL(clicked()), this, SLOT(editLandmarkButtonClickedSlot()));

    mRemoveLandmarkButton = new QPushButton("Clear", this);
    mRemoveLandmarkButton->setToolTip("Clear selected landmark");
    mRemoveLandmarkButton->setDisabled(true);
    connect(mRemoveLandmarkButton, SIGNAL(clicked()), this, SLOT(removeLandmarkButtonClickedSlot()));

    mCreateCenterlineButton = new QPushButton("Create centerline", this);
    mCreateCenterlineButton->setToolTip("Create centerline from landmarks");
    mCreateCenterlineButton->setDisabled(true);
    connect(mCreateCenterlineButton, SIGNAL(clicked()), this, SLOT(createCenterlineButtonClickedSlot()));

	//layout
	mVerticalLayout->addWidget(new LabeledComboBoxWidget(this, mCurrentProperty));
	mVerticalLayout->addWidget(mLandmarkTableWidget);
    mVerticalLayout->addWidget(mAvarageAccuracyLabel);

	QHBoxLayout* landmarkButtonsLayout = new QHBoxLayout;
	landmarkButtonsLayout->addWidget(mAddLandmarkButton);
	landmarkButtonsLayout->addWidget(mEditLandmarkButton);
    landmarkButtonsLayout->addWidget(mRemoveLandmarkButton);
    landmarkButtonsLayout->addWidget(mCreateCenterlineButton);
	mVerticalLayout->addLayout(landmarkButtonsLayout);
}

CenterlinePointsWidget::~CenterlinePointsWidget()
{
}

void CenterlinePointsWidget::onCurrentImageChanged()
{
	DataPtr data = mCurrentProperty->getData();

	mLandmarkListener->setLandmarkSource(data);
	this->enableButtons();

	if (data && !mServices->registration()->getFixedData())
		mServices->registration()->setFixedData(data);

	this->setModified();
}

PickerRepPtr CenterlinePointsWidget::getPickerRep()
{
	return mServices->view()->get3DReps(0, 0)->findFirst<PickerRep>();
}

DataPtr CenterlinePointsWidget::getCurrentData() const
{
	return mLandmarkListener->getLandmarkSource();
}

void CenterlinePointsWidget::addLandmarkButtonClickedSlot()
{
	PickerRepPtr PickerRep = this->getPickerRep();
	if (!PickerRep)
	{
		reportError("Need a 3D view to set landmarks.");
		return;
	}

	DataPtr image = this->getCurrentData();
	if (!image)
		return;

	QString uid = mServices->patient()->addLandmark();
	Vector3D pos_r = PickerRep->getPosition();
	Vector3D pos_d = image->get_rMd().inv().coord(pos_r);
	image->getLandmarks()->setLandmark(Landmark(uid, pos_d));

    this->activateLandmark(uid);
}


void CenterlinePointsWidget::editLandmarkButtonClickedSlot()
{
	PickerRepPtr PickerRep = this->getPickerRep();
	if (!PickerRep)
	{
		reportError("Need a 3D view to edit landmarks.");
		return;
	}

	DataPtr image = this->getCurrentData();
	if (!image)
		return;

	QString uid = mActiveLandmark;
	Vector3D pos_r = PickerRep->getPosition();
	Vector3D pos_d = image->get_rMd().inv().coord(pos_r);
	image->getLandmarks()->setLandmark(Landmark(uid, pos_d));

    this->activateLandmark(this->getNextLandmark());
}

void CenterlinePointsWidget::removeLandmarkButtonClickedSlot()
{
    DataPtr image = this->getCurrentData();
    if (!image)
        return;

    QString next = this->getNextLandmark();
    image->getLandmarks()->removeLandmark(mActiveLandmark);
    this->activateLandmark(next);
}

void CenterlinePointsWidget::createCenterlineButtonClickedSlot()
{
	DataPtr image = this->getCurrentData();
	if (!image)
		return;

    Transform3D rMd = image->get_rMd();

    std::vector<Landmark> landmarkVector = this-> getAllLandmarks();

    vtkPolyDataPtr outputPositions = vtkPolyDataPtr::New();
    vtkPointsPtr points = vtkPointsPtr::New();
    vtkCellArrayPtr lines = vtkCellArrayPtr::New();

    for(int i=0; i<landmarkVector.size(); i++)
    {
        LandmarkProperty prop = mServices->patient()->getLandmarkProperties()[landmarkVector[i].getUid()];
        if (prop.getActive())
        {
            Vector3D pos = landmarkVector[i].getCoord();
            pos = rMd.coord(pos);
            points->InsertNextPoint(pos(0),pos(1),pos(2));
        }
    }
    for(int i=0; i<points->GetNumberOfPoints()-1; i++)
    {
        vtkIdType connection[2] = {i, i+1};
        lines->InsertNextCell(2, connection);
    }

    outputPositions->SetPoints(points);
    outputPositions->SetLines(lines);

    QString filename;
    QString format = timestampSecondsFormatNice();
    filename = "CenterlineFromPoints " + QDateTime::currentDateTime().toString(format);

    MeshPtr mesh = mServices->patient()->createSpecificData<Mesh>(filename, filename);
    mesh->setVtkPolyData(outputPositions);
    mesh->setColor(QColor(0, 0, 255, 255));
    mServices->patient()->insertData(mesh);
    mServices->view()->autoShowData(mesh);
}

void CenterlinePointsWidget::cellClickedSlot(int row, int column)
{
	LandmarkRegistrationWidget::cellClickedSlot(row, column);
	this->enableButtons();
}

void CenterlinePointsWidget::enableButtons()
{
	bool selected = !mLandmarkTableWidget->selectedItems().isEmpty();
	bool loaded = this->getCurrentData() != 0;

	mEditLandmarkButton->setEnabled(selected);
    mRemoveLandmarkButton->setEnabled(selected);
    mCreateCenterlineButton->setEnabled(selected);
	mAddLandmarkButton->setEnabled(loaded);

	DataPtr image = this->getCurrentData();
	if (image)
	{
		mAddLandmarkButton->setToolTip(QString("Add landmark to image %1").arg(image->getName()));
		mEditLandmarkButton->setToolTip(QString("Resample landmark in image %1").arg(image->getName()));
	}
//	this->setModified();
}

void CenterlinePointsWidget::showEvent(QShowEvent* event)
{
	mServices->view()->setRegistrationMode(rsIMAGE_REGISTRATED);
	LandmarkRegistrationWidget::showEvent(event);

	if(!mUseRegistrationFixedPropertyInsteadOfActiveImage)
	{
		ActiveDataPtr activeData = mServices->patient()->getActiveData();
		ImagePtr image = activeData->getActive<Image>();
		if (image)
			mCurrentProperty->setValue(image->getUid());
	}
}

void CenterlinePointsWidget::hideEvent(QHideEvent* event)
{
	mServices->view()->setRegistrationMode(rsNOT_REGISTRATED);
	LandmarkRegistrationWidget::hideEvent(event);

}

void CenterlinePointsWidget::prePaintEvent()
{
    LandmarkRegistrationWidget::prePaintEvent();

	std::vector<Landmark> landmarks = this->getAllLandmarks();

	//update buttons
    mCreateCenterlineButton->setEnabled(!landmarks.empty() && !mActiveLandmark.isEmpty());
	mEditLandmarkButton->setEnabled(!landmarks.empty() && !mActiveLandmark.isEmpty());
}

LandmarkMap CenterlinePointsWidget::getTargetLandmarks() const
{
	DataPtr image = this->getCurrentData();
	if (!image)
		return LandmarkMap();

	return image->getLandmarks()->getLandmarks();
}

/** Return transform from target space to reference space
 *
 */
Transform3D CenterlinePointsWidget::getTargetTransform() const
{
	DataPtr image = this->getCurrentData();
	if (!image)
		return Transform3D::Identity();
	return image->get_rMd();
}

void CenterlinePointsWidget::setTargetLandmark(QString uid, Vector3D p_target)
{
	DataPtr image = this->getCurrentData();
	if (!image)
		return;
	image->getLandmarks()->setLandmark(Landmark(uid, p_target));
}

QString CenterlinePointsWidget::getTargetName() const
{
	DataPtr image = this->getCurrentData();
	if (!image)
		return "None";
	return image->getName();
}


}//namespace cx
