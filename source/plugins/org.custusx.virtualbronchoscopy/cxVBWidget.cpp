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
#include <QTimer>

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
#include "cxVLCRecorder.h"



namespace cx
{

VBWidget::VBWidget(VisServicesPtr services, QWidget *parent) :
	QWidget(parent),
	mServices(services),
	mVerticalLayout(new QVBoxLayout(this)),
	mControlsEnabled(false),
	mAutomaticRotation(true),
	mRecordVideo(false),
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
							boost::bind(&StringPropertySelectMesh::setValue, mRouteToTarget, boost::placeholders::_1));

	// Selector for route to target
	QVBoxLayout *inputVbox = new QVBoxLayout;
	inputVbox->addWidget(new DataSelectWidget(services->view(), services->patient(), this,mRouteToTarget));
	QGroupBox *inputBox = new QGroupBox(tr("Input"));
	inputBox->setLayout(inputVbox);
	mVerticalLayout->addWidget(inputBox);

	// play/pause button
	mPlayButton = new QPushButton(QIcon(":/icons/open_icon_library/media-playback-start-3.png"),"");
	mTimer = new QTimer;
	connect(mTimer, SIGNAL(timeout()), this, SLOT(moveCameraSlot()));
	mTimer->setInterval(20); // slot processing time is about 30 ms, thus about 50 ms in total.

	// Selectors for position along path and play/pause
	QHBoxLayout *playbackHBox = new QHBoxLayout;
	QGroupBox	*playbackBox = new QGroupBox(tr("Playback"));
	mPlaybackSlider = new QSlider(Qt::Horizontal);
	QLabel		*labelStart = new QLabel(tr("Start (Q/A)"));
	QLabel		*labelTarget = new QLabel(tr(" Target"));
	playbackHBox->addWidget(mPlayButton);
	playbackHBox->addWidget(labelStart);
	playbackHBox->addWidget(mPlaybackSlider);
	playbackHBox->addWidget(labelTarget);
	playbackBox->setLayout(playbackHBox);
	mVerticalLayout->addWidget(playbackBox);
	mPlaybackSlider->setMinimum(0);
	mPlaybackSlider->setMaximum(1000);

	// Selectors for virtual endoscope control
	QGroupBox	*endoscopeBox = new QGroupBox(tr("Bronchoscope"));
	QGridLayout	*endoscopeControlLayout = new QGridLayout;
	QGridLayout	*endoscopeButtonsLayout = new QGridLayout;
	QLabel		*labelRot = new QLabel(tr("Rotate (W/S)"));
	QLabel		*labelViewLeftRight = new QLabel(tr("Left/right (R/F)"));
	QLabel		*labelViewUpDown = new QLabel(tr("Up/Down (E/D)"));
	mRotateDial = new QDial;
	mViewDialLeftRight = new QDial;
	mViewDialUpDown = new QDial;
	mResetEndoscopeButton = new QPushButton("Reset");
	mUseAutomaticRotationButton = new QPushButton("Automatic rotation");
	mAutomaticRotationButtonBackgroundColor = mUseAutomaticRotationButton->palette();
	mAutomaticRotationButtonBackgroundColor.setColor(QPalette::Button, Qt::green);
	mUseAutomaticRotationButton->setPalette(mAutomaticRotationButtonBackgroundColor);

	resetEndoscopeSlot();

	endoscopeControlLayout->addWidget(labelRot,0,0,Qt::AlignHCenter);
	endoscopeControlLayout->addWidget(labelViewUpDown,0,1,Qt::AlignHCenter);
	endoscopeControlLayout->addWidget(labelViewLeftRight,0,2,Qt::AlignHCenter);
	endoscopeControlLayout->addWidget(mRotateDial,1,0);
	endoscopeControlLayout->addWidget(mViewDialUpDown,1,1);
	endoscopeControlLayout->addWidget(mViewDialLeftRight,1,2);
	endoscopeButtonsLayout->addWidget(mResetEndoscopeButton,0,0);
	endoscopeButtonsLayout->addWidget(mUseAutomaticRotationButton,0,1);
	endoscopeControlLayout->addLayout(endoscopeButtonsLayout,2,0,1,3);
	endoscopeBox->setLayout(endoscopeControlLayout);
	mVerticalLayout->addWidget(endoscopeBox);

	this->setLayout(mVerticalLayout);


	this->enableControls(false);

	mCameraPath = new CXVBcameraPath(services->tracking(), services->patient(), services->view());

	connect(mRouteToTarget.get(), &SelectDataStringPropertyBase::dataChanged,
			this, &VBWidget::inputChangedSlot, Qt::UniqueConnection);
	connect(this, &VBWidget::cameraPathChanged, mCameraPath, &CXVBcameraPath::cameraRawPointsSlot);
	connect(mPlaybackSlider, &QSlider::valueChanged, mCameraPath, &CXVBcameraPath::cameraPathPositionSlot);
	connect(mPlayButton, &QPushButton::clicked, this, &VBWidget::playButtonClickedSlot);
	connect(mViewDialLeftRight, &QSlider::valueChanged, mCameraPath, &CXVBcameraPath::cameraViewAngleXSlot);
	connect(mViewDialUpDown, &QSlider::valueChanged, mCameraPath, &CXVBcameraPath::cameraViewAngleYSlot);
	connect(mRotateDial, &QDial::valueChanged, mCameraPath, &CXVBcameraPath::cameraRotateAngleSlot);
	connect(mResetEndoscopeButton, &QPushButton::clicked, this, &VBWidget::resetEndoscopeSlot);
	connect(mUseAutomaticRotationButton, &QPushButton::clicked, this, &VBWidget::automaticRotationSlot);
	connect(mCameraPath, &CXVBcameraPath::rotationChanged, this, &VBWidget::updateRotationDialSlot);

	mVerticalLayout->addStretch();
}

VBWidget::~VBWidget()
{
	delete mTimer;
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

void VBWidget::setBranchingIndexAlongRoute(std::vector< int > branchingIndex)
{
	mCameraPath->setBranchingIndexAlongRoute(branchingIndex);
}

void VBWidget::setRecordVideoOption(bool recordVideo)
{
	mRecordVideo = recordVideo;
}

QFileInfo VBWidget::startRecordFullscreen()
{
	QFileInfo fileInfo;
	fileInfo.setFile(mServices->patient()->generateFilePath("Screenshots", "mp4"));
	if(!vlc()->isRecording())
		vlc()->startRecording(fileInfo.absoluteFilePath());
	return fileInfo;
}

void VBWidget::stopRecordFullscreen()
{
	if(vlc()->isRecording())
		vlc()->stopRecording();
	vlc()->waitForFinished();
}

void  VBWidget::enableControls(bool enable)
{
	mPlaybackSlider->setEnabled(enable);
	mRotateDial->setEnabled(enable);
	mViewDialLeftRight->setEnabled(enable);
	mViewDialUpDown->setEnabled(enable);
	mControlsEnabled = enable;
}

void VBWidget::inputChangedSlot()
{
	this->enableControls(true);
	emit cameraPathChanged(mRouteToTarget->getMesh());
}

void VBWidget::keyPressEvent(QKeyEvent* event)
{
	if (event->key()==Qt::Key_Up || event->key()==Qt::Key_8 || event->key()==Qt::Key_Q)
	{
		if(mControlsEnabled) {
			int currentPos = mPlaybackSlider->value();
			mPlaybackSlider->setValue(currentPos+1);
			return;
		}
	}

	if (event->key()==Qt::Key_Down || event->key()==Qt::Key_2 || event->key()==Qt::Key_A)
	{
		if(mControlsEnabled) {
			int currentPos = mPlaybackSlider->value();
			mPlaybackSlider->setValue(currentPos-1);
			return;
		}
	}

	if (event->key()==Qt::Key_Right || event->key()==Qt::Key_6 || event->key()==Qt::Key_R)
	{
		if(mControlsEnabled) {
			int currentPos = mViewDialLeftRight->value();
			mViewDialLeftRight->setValue(currentPos+1);
			return;
		}
	}

	if (event->key()==Qt::Key_Left || event->key()==Qt::Key_4 || event->key()==Qt::Key_F)
	{
		if(mControlsEnabled) {
			int currentPos = mViewDialLeftRight->value();
			mViewDialLeftRight->setValue(currentPos-1);
			return;
		}
	}

	if (event->key()==Qt::Key_D)
	{
		if(mControlsEnabled) {
			int currentPos = mViewDialUpDown->value();
			mViewDialUpDown->setValue(currentPos+1);
			return;
		}
	}

	if (event->key()==Qt::Key_E)
	{
		if(mControlsEnabled) {
			int currentPos = mViewDialUpDown->value();
			mViewDialUpDown->setValue(currentPos-1);
			return;
		}
	}

	if (event->key()==Qt::Key_9|| event->key()==Qt::Key_W)
	{
		if(mControlsEnabled) {
			int currentPos = mRotateDial->value();
			mRotateDial->setValue(currentPos+2);
			return;
		}
	}

	if (event->key()==Qt::Key_3|| event->key()==Qt::Key_S)
	{
		if(mControlsEnabled) {
			int currentPos = mRotateDial->value();
			mRotateDial->setValue(currentPos-2);
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

void VBWidget::playButtonClickedSlot()
{
	if(mTimer->isActive())
	{
		mTimer->stop();
		if(mRecordVideo)
		{
			this->stopRecordFullscreen();
			mCameraPath->setWritePositionsToFile(false);
		}
		mPlayButton->setIcon(QIcon(":/icons/open_icon_library/media-playback-start-3.png"));
	}
	else
	{
		mTimer->start();
		if(mRecordVideo)
		{
			QFileInfo fileInfo = this->startRecordFullscreen();
			mCameraPath->setWritePositionsFilePath(fileInfo.absolutePath() + "/" + fileInfo.baseName());
			mCameraPath->setWritePositionsToFile(true);
		}
			mPlayButton->setIcon(QIcon(":/icons/open_icon_library/media-playback-pause-3.png"));
	}
}

void VBWidget::moveCameraSlot()
{
	int currentPos = mPlaybackSlider->value();
	if(currentPos >= mPlaybackSlider->maximum())
	{
		this->playButtonClickedSlot();
		emit cameraAtEndPosition();
		return;
	}
	mPlaybackSlider->setValue(currentPos+1);
}

void VBWidget::resetEndoscopeSlot()
{
	mRotateDial->setMinimum(-180);
	mRotateDial->setMaximum(180);
	mViewDialLeftRight->setMinimum(-60);
	mViewDialLeftRight->setMaximum(60);
	mViewDialUpDown->setMinimum(-60);
	mViewDialUpDown->setMaximum(60);
	mRotateDial->setValue(0);
	mViewDialLeftRight->setValue(0);
	mViewDialUpDown->setValue(0);
	mResetEndoscopeButton->show();
	mUseAutomaticRotationButton->show();
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

void VBWidget::disableAutomaticRotation()
{
	if(mAutomaticRotation)
		this->automaticRotationSlot();
}

QString VBWidget::defaultWhatsThis() const
{
return "<html>"
			 "<h3>Virtual Bronchoscopy.</h3>"
			 "<p>GUI for visualizing a route-to-target path</p>"
			 "</html>";
}



} /* namespace cx */
