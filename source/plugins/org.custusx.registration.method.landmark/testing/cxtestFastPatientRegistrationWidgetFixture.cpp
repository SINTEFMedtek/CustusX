/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxtestFastPatientRegistrationWidgetFixture.h"
#include <QShowEvent>
#include <QHideEvent>
//#include "cxVector3D.h"


cxtest::FastPatientRegistrationWidgetFixture::FastPatientRegistrationWidgetFixture(cx::RegServicesPtr services, QWidget *parent) :
	cx::FastPatientRegistrationWidget(services, parent),
	mPointSampled(false)
{}

void cxtest::FastPatientRegistrationWidgetFixture::triggerShowEvent()
{
	this->showEvent(new QShowEvent());
}

void cxtest::FastPatientRegistrationWidgetFixture::triggerHideEvent()
{
	this->hideEvent(new QHideEvent());
}

QCheckBox* cxtest::FastPatientRegistrationWidgetFixture::getMouseClickSample()
{
	return mMouseClickSample;
}

void cxtest::FastPatientRegistrationWidgetFixture::pointSampled(cx::Vector3D p_r)
{
	mPointSampled = true;
}
