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

#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QSlider>
#include <QSpinBox>
#include <QGridLayout>
#include <QLabel>
#include <QDial>

#include "cxVBWidget.h"
#include "cxPatientModelServiceProxy.h"
#include "cxViewServiceProxy.h"
#include "cxDataSelectWidget.h"
#include "cxTrackingServiceProxy.h"
#include "cxView.h"
#include "cxSessionStorageServiceProxy.h"
#include "cxPatientStorage.h"
#include "cxVisServices.h"



namespace cx
{

VBWidget::VBWidget(VisServicesPtr services, QWidget *parent) :
	QWidget(parent),
	mVerticalLayout(new QVBoxLayout(this)),
	mControlsEnabled(false),
	mStorage(new PatientStorage(services->session(), "VirtualBronchoscopy"))
{
	this->setObjectName("Virtual Bronchoscopy Widget");
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
//	QSpinBox	*speedSpinbox = new QSpinBox();
	playbackHBox->addWidget(labelStart);
	playbackHBox->addWidget(mPlaybackSlider);
	playbackHBox->addWidget(labelTarget);
//	playbackHBox->addWidget(speedSpinbox);
	playbackBox->setLayout(playbackHBox);
	mVerticalLayout->addWidget(playbackBox);
	mPlaybackSlider->setMinimum(0);
	mPlaybackSlider->setMaximum(100);

	// Selectors for virtual endoscope control
	QGroupBox	*endoscopeBox = new QGroupBox(tr("Endoscope"));
	QGridLayout	*endoscopeControlLayout = new QGridLayout;
	QLabel		*labelRot = new QLabel(tr("Rotate"));
	QLabel		*labelView = new QLabel(tr("Left/right (30 deg)"));
	mRotateDial = new QDial;
	mRotateDial->setMinimum(-180);
	mRotateDial->setMaximum(180);
	mViewSlider = new QDial;
	mViewSlider->setMinimum(-30);
	mViewSlider->setMaximum(30);


	endoscopeControlLayout->addWidget(labelRot,0,0,Qt::AlignHCenter);
	endoscopeControlLayout->addWidget(labelView,0,2,Qt::AlignHCenter);
	endoscopeControlLayout->addWidget(mRotateDial,1,0);
	endoscopeControlLayout->addWidget(mViewSlider,1,2);
	endoscopeBox->setLayout(endoscopeControlLayout);
	mVerticalLayout->addWidget(endoscopeBox);


	this->enableControls(false);

	mCameraPath = new CXVBcameraPath(services->tracking(), services->patient(), services->view());

	connect(mRouteToTarget.get(), &SelectDataStringPropertyBase::dataChanged,
			this, &VBWidget::inputChangedSlot);
	connect(this, &VBWidget::cameraPathChanged, mCameraPath, &CXVBcameraPath::cameraRawPointsSlot);
	connect(mPlaybackSlider, &QSlider::valueChanged, mCameraPath, &CXVBcameraPath::cameraPathPositionSlot);
	connect(mViewSlider, &QSlider::valueChanged, mCameraPath, &CXVBcameraPath::cameraViewAngleSlot);
	connect(mRotateDial, &QDial::valueChanged, mCameraPath, &CXVBcameraPath::cameraRotateAngleSlot);

	mVerticalLayout->addStretch();
//	mHorizontalLayout->addStretch();
//	mHorizontalLayout->addWidget(new QLabel("Virtual Bronchoscopy functionality ..."));
}

VBWidget::~VBWidget()
{
}

void  VBWidget::enableControls(bool enable)
{
	mPlaybackSlider->setEnabled(enable);
	mRotateDial->setEnabled(enable);
	mViewSlider->setEnabled(enable);
	mControlsEnabled = enable;
}

void VBWidget::inputChangedSlot()
{
	this->enableControls(true);
	emit cameraPathChanged(mRouteToTarget->getMesh());
}

void VBWidget::keyPressEvent(QKeyEvent* event)
{

	if (event->key()==Qt::Key_Up)
	{
		if(mControlsEnabled) {
			int currentPos = mPlaybackSlider->value();
			mPlaybackSlider->setValue(currentPos+1);
			return;
		}
	}

	if (event->key()==Qt::Key_Down)
	{
		if(mControlsEnabled) {
			int currentPos = mPlaybackSlider->value();
			mPlaybackSlider->setValue(currentPos-1);
			return;
		}
	}

	if (event->key()==Qt::Key_Right)
	{
		if(mControlsEnabled) {
			int currentPos = mViewSlider->value();
			mViewSlider->setValue(currentPos+1);
			return;
		}
	}

	if (event->key()==Qt::Key_Left)
	{
		if(mControlsEnabled) {
			int currentPos = mViewSlider->value();
			mViewSlider->setValue(currentPos-1);
			return;
		}
	}

	// Forward the keyPressevent if not processed
	QWidget::keyPressEvent(event);
}

QString VBWidget::defaultWhatsThis() const
{
  return "<html>"
	  "<h3>Virtual Bronchoscopy.</h3>"
	  "<p>GUI for visualizing a route-to-target path</p>"
      "</html>";
}



} /* namespace cx */
