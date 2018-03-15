/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXICPWIDGET_H
#define CXICPWIDGET_H

#include "cxRegistrationBaseWidget.h"
#include "cxStringPropertyBase.h"
#include "vesselReg/SeansVesselReg.hxx"
#include "cxBoolProperty.h"
#include "cxDoubleProperty.h"
#include "org_custusx_registration_method_vessel_Export.h"

class QSpinBox;
class QPushButton;
class QLabel;

namespace cx
{
//typedef boost::shared_ptr<class SeansVesselRegistrationDebugger> SeansVesselRegistrationDebuggerPtr;

/** GUI for ICP control - no logic.
 *
 * \ingroup org_custusx_registration_method_vessel
 * \date 2015-09-13
 * \author Christian Askeland
 */
class org_custusx_registration_method_vessel_EXPORT ICPWidget : public BaseWidget
{
	Q_OBJECT
public:
	ICPWidget(QWidget* parent);
	virtual ~ICPWidget();

	void setSettings(std::vector<PropertyPtr> properties);
	void enableRegistration(bool on);
	void setRMS(double val);

signals:
	void requestRegister();

private:
	QWidget* createOptionsWidget();

	std::vector<PropertyPtr> mProperties;
	QPushButton* mRegisterButton;
	QAction* mVesselRegOptionsButton;
	QWidget* mOptionsWidget;
	QGroupBox* mVesselRegOptionsWidget;
	QLineEdit* mMetricValue;
};



}//namespace cx

#endif // CXICPWIDGET_H
