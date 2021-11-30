/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXTESTFASTPATIENTREGISTRATIONWIDGETFIXTURE_H
#define CXTESTFASTPATIENTREGISTRATIONWIDGETFIXTURE_H

#include "cxFastPatientRegistrationWidget.h"
#include "cxtest_org_custusx_registration_method_landmark_export.h"

namespace cx
{
typedef boost::shared_ptr<class ViewWrapper2D> ViewWrapper2DPtr;
}

namespace cxtest
{

typedef boost::shared_ptr<class FastPatientRegistrationWidgetFixture> FastPatientRegistrationWidgetFixturePtr;
class CXTEST_ORG_CUSTUSX_REGISTRATION_METHOD_LANDMARK_EXPORT FastPatientRegistrationWidgetFixture : public cx::FastPatientRegistrationWidget
{
	Q_OBJECT
public:
	FastPatientRegistrationWidgetFixture(cx::RegServicesPtr services, QWidget* parent);
	void connectTo(cx::ViewWrapper2DPtr viewWrapper);

	void triggerShowEvent();
	void triggerHideEvent();
	QCheckBox *getMouseClickSample();
	bool mPointSampled;
	QString getNextLandmark();
protected slots:
	virtual void pointSampled(cx::Vector3D p_r);
};

}//namespace cxtest

#endif // CXTESTFASTPATIENTREGISTRATIONWIDGETFIXTURE_H
