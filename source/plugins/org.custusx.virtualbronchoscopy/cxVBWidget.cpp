/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QSlider>
#include <QSpinBox>
#include <QGridLayout>
#include <QLabel>
#include <QDial>
#include <QPushButton>

#include "cxVBWidget.h"
#include "cxPatientModelServiceProxy.h"
#include "cxViewServiceProxy.h"
#include "cxDataSelectWidget.h"
#include "cxTrackingServiceProxy.h"
#include "cxView.h"
#include "cxSessionStorageServiceProxy.h"
#include "cxPatientStorage.h"
#include "cxVisServices.h"
#include "cxLogger.h"
#include "cxRouteToTarget.h"



namespace cx
{

VBWidget::VBWidget(VisServicesPtr services, QWidget *parent) :
	QWidget(parent),
	mVerticalLayout(new QVBoxLayout(this)),
    mControlsEnabled(false),
    mAutomaticRotation(true),
	mStorage(new PatientStorage(services->session(), "VirtualBronchoscopy"))
{
	this->setObjectName("virtual_bronchoscopy_widget");
	this->setWindowTitle("Virtual Bronchoscopy");
	this->setWhatsThis(this->defaultWhatsThis());

	this->setFocusPolicy(Qt::StrongFocus);  // Widget needs focus to handle Key events

	mRouteToTarget = StringPropertySelectMesh::New(services->patient());
	mRouteToTarget->setValueName("Route to target path: ");
	mStorage->storeVariable("routeToTarget",
							boost::bind(&StringPropertySelectMesh::getValue, mRouteToTarget),
							boost::bind(&StringPropertySelectMesh::setValue, mRouteToTarget, _1));

	// Selector for route to target
	QVBoxLayout *inputVbox = new QVBoxLayout;
	inputVbox->addWidget(new DataSelectWidget(services->view(), services->patient(), this,mRouteToTarget));
	QGroupBox *inputBox = new QGroupBox(tr("Input"));
	inputBox->setLayout(inputVbox);
	mVerticalLayout->addWidget(inputBox);

	// Selectors for position along path and play/pause
	QHBoxLayout *playbackHBox = new QHBoxLayout;
	QGroupBox	*playbackBox = new QGroupBox(tr("Playback"));
	mPlaybackSlider = new QSlider(Qt::Horizontal);
	QLabel		*labelStart = new QLabel(tr("Start "));
	QLabel		*labelTarget = new QLabel(tr(" Target"));
	playbackHBox->addWidget(labelStart);
	playbackHBox->addWidget(mPlaybackSlider);
	playbackHBox->addWidget(labelTarget);
	playbackBox->setLayout(playbackHBox);
	mVerticalLayout->addWidget(playbackBox);
	mPlaybackSlider->setMinimum(0);
	mPlaybackSlider->setMaximum(100);

	// Selectors for virtual endoscope control
	QGroupBox	*endoscopeBox = new QGroupBox(tr("Bronchoscope"));
	QGridLayout	*endoscopeControlLayout = new QGridLayout;
	QLabel		*labelRot = new QLabel(tr("Rotate (360 deg)"));
	QLabel		*labelView = new QLabel(tr("Left/right (+/- 60 deg)"));
	mRotateDial = new QDial;
	mRotateDial->setMinimum(-180);
	mRotateDial->setMaximum(180);
	mViewDial = new QDial;
	mViewDial->setMinimum(-60);
	mViewDial->setMaximum(60);
	mResetEndoscopeButton = new QPushButton("Reset");
    mUseAutomaticRotationButton = new QPushButton("Automatic rotation");
    mAutomaticRotationButtonBackgroundColor = mUseAutomaticRotationButton->palette();
    mAutomaticRotationButtonBackgroundColor.setColor(QPalette::Button, Qt::green);
    mUseAutomaticRotationButton->setPalette(mAutomaticRotationButtonBackgroundColor);

	endoscopeControlLayout->addWidget(labelRot,0,0,Qt::AlignHCenter);
	endoscopeControlLayout->addWidget(labelView,0,2,Qt::AlignHCenter);
	endoscopeControlLayout->addWidget(mRotateDial,1,0);
	endoscopeControlLayout->addWidget(mViewDial,1,2);
	endoscopeControlLayout->addWidget(mResetEndoscopeButton,2,0);
    endoscopeControlLayout->addWidget(mUseAutomaticRotationButton,3,0);
	endoscopeBox->setLayout(endoscopeControlLayout);
	mVerticalLayout->addWidget(endoscopeBox);

	this->setLayout(mVerticalLayout);


	this->enableControls(false);

	mCameraPath = new CXVBcameraPath(services->tracking(), services->patient(), services->view());

	connect(mRouteToTarget.get(), &SelectDataStringPropertyBase::dataChanged,
			this, &VBWidget::inputChangedSlot, Qt::UniqueConnection);
	connect(this, &VBWidget::cameraPathChanged, mCameraPath, &CXVBcameraPath::cameraRawPointsSlot);
	connect(mPlaybackSlider, &QSlider::valueChanged, mCameraPath, &CXVBcameraPath::cameraPathPositionSlot);
	connect(mViewDial, &QSlider::valueChanged, mCameraPath, &CXVBcameraPath::cameraViewAngleSlot);
	connect(mRotateDial, &QDial::valueChanged, mCameraPath, &CXVBcameraPath::cameraRotateAngleSlot);
	connect(mResetEndoscopeButton, &QPushButton::clicked, this, &VBWidget::resetEndoscopeSlot);
    connect(mUseAutomaticRotationButton, &QPushButton::clicked, this, &VBWidget::automaticRotationSlot);
    connect(mCameraPath, &CXVBcameraPath::rotationChanged, this, &VBWidget::updateRotationDialSlot);

	mVerticalLayout->addStretch();
}

VBWidget::~VBWidget()
{
}

void VBWidget::setRouteToTarget(QString uid)
{
	disconnect(mRouteToTarget.get(), &SelectDataStringPropertyBase::dataChanged, this, &VBWidget::inputChangedSlot);
	mRouteToTarget->setValue("");
	connect(mRouteToTarget.get(), &SelectDataStringPropertyBase::dataChanged, this, &VBWidget::inputChangedSlot, Qt::UniqueConnection);
	mRouteToTarget->setValue(uid);

	disconnect(mPlaybackSlider, &QSlider::valueChanged, mCameraPath, &CXVBcameraPath::cameraPathPositionSlot);
	mPlaybackSlider->setValue(1);
	connect(mPlaybackSlider, &QSlider::valueChanged, mCameraPath, &CXVBcameraPath::cameraPathPositionSlot, Qt::UniqueConnection);
	mPlaybackSlider->setValue(5);
}

void VBWidget::setRoutePositions(std::vector< Eigen::Vector3d > routePositions)
{
     mCameraPath->setRoutePositions(routePositions);
}

void VBWidget::setCameraRotationAlongRoute(std::vector< double > cameraRotations)
{
    mCameraPath->setCameraRotations(cameraRotations);
}

void  VBWidget::enableControls(bool enable)
{
	mPlaybackSlider->setEnabled(enable);
	mRotateDial->setEnabled(enable);
	mViewDial->setEnabled(enable);
	mControlsEnabled = enable;
}

void VBWidget::inputChangedSlot()
{
	this->enableControls(true);
	emit cameraPathChanged(mRouteToTarget->getMesh());
}

void VBWidget::keyPressEvent(QKeyEvent* event)
{
	if (event->key()==Qt::Key_Up || event->key()==Qt::Key_8)
	{
		if(mControlsEnabled) {
			int currentPos = mPlaybackSlider->value();
			mPlaybackSlider->setValue(currentPos+1);
			return;
		}
	}

	if (event->key()==Qt::Key_Down || event->key()==Qt::Key_2)
	{
		if(mControlsEnabled) {
			int currentPos = mPlaybackSlider->value();
			mPlaybackSlider->setValue(currentPos-1);
			return;
		}
	}

	if (event->key()==Qt::Key_Right || event->key()==Qt::Key_6)
	{
		if(mControlsEnabled) {
            int currentPos = mViewDial->value();
            mViewDial->setValue(currentPos+1);
			return;
		}
	}

	if (event->key()==Qt::Key_Left || event->key()==Qt::Key_4)
	{
		if(mControlsEnabled) {
            int currentPos = mViewDial->value();
            mViewDial->setValue(currentPos-1);
			return;
		}
	}

	if (event->key()==Qt::Key_PageUp || event->key()==Qt::Key_9)
	{
		if(mControlsEnabled) {
            int currentPos = mRotateDial->value();
            mRotateDial->setValue(currentPos+1);
			return;
		}
	}

	if (event->key()==Qt::Key_PageDown || event->key()==Qt::Key_3)
	{
		if(mControlsEnabled) {
            int currentPos = mRotateDial->value();
            mRotateDial->setValue(currentPos-1);
			return;
		}
	}

	if (event->key()==Qt::Key_5)
	{
		if(mControlsEnabled) {
			this->resetEndoscopeSlot();
			return;
		}
	}

	// Forward the keyPressevent if not processed
	QWidget::keyPressEvent(event);
}

void VBWidget::resetEndoscopeSlot()
{
	mRotateDial->setValue(0);
	mViewDial->setValue(0);
}

void VBWidget::automaticRotationSlot()
{
    mAutomaticRotation = !mAutomaticRotation;
    mCameraPath->setAutomaticRotation(mAutomaticRotation);
    if(mAutomaticRotation)
    {
        mAutomaticRotationButtonBackgroundColor.setColor(QPalette::Button, Qt::green);
        mUseAutomaticRotationButton->setPalette(mAutomaticRotationButtonBackgroundColor);
    }
    else
    {
        mAutomaticRotationButtonBackgroundColor.setColor(QPalette::Button, Qt::gray);
        mUseAutomaticRotationButton->setPalette(mAutomaticRotationButtonBackgroundColor);
    }
}

void VBWidget::updateRotationDialSlot(int value)
{
    mRotateDial->setValue(value);
}

QString VBWidget::defaultWhatsThis() const
{
  return "<html>"
	  "<h3>Virtual Bronchoscopy.</h3>"
	  "<p>GUI for visualizing a route-to-target path</p>"
      "</html>";
}



} /* namespace cx */
