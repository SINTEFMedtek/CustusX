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
#ifndef CXICPREGISTRATIONBASEWIDGET_H_
#define CXICPREGISTRATIONBASEWIDGET_H_

#include "cxRegistrationBaseWidget.h"
#include "cxStringPropertyBase.h"

class QSpinBox;
class QPushButton;
class QLabel;
#include "cxBoolProperty.h"
#include "cxDoubleProperty.h"
#include "cxTransform3D.h"
#include "org_custusx_registration_method_vessel_Export.h"

namespace cx
{
class ICPWidget;
typedef boost::shared_ptr<class SeansVesselRegistrationDebugger> SeansVesselRegistrationDebuggerPtr;
typedef boost::shared_ptr<class GeometricRep> GeometricRepPtr;
typedef boost::shared_ptr<class SeansVesselReg> SeansVesselRegPtr;
typedef boost::shared_ptr<class MeshInView> MeshInViewPtr;
typedef boost::shared_ptr<class SpaceListener> SpaceListenerPtr;




/**
 *
 * Base class for Widgets implementing registration using the ICP method.
 * Subclass to perform e.g. I2I or I2P registration.
 *
 * Note: mICPWidget will be created and initialized, but not added to a
 * widget. Must be done by subclass.
 *
 * \ingroup org_custusx_registration_method_vessel
 * \date Feb 21, 2011
 * \author Janne Beate Bakeng, SINTEF
 */
class org_custusx_registration_method_vessel_EXPORT ICPRegistrationBaseWidget : public RegistrationBaseWidget
{
	Q_OBJECT
public:
	ICPRegistrationBaseWidget(RegServicesPtr services, QWidget* parent, QString uid, QString name);
	virtual ~ICPRegistrationBaseWidget();

protected:
	/**
	 * subclass must call mRegistrator->initialize() with proper input
	 */
	virtual void initializeRegistrator() = 0;
	/**
	 * called when widget is shown. Subclass must update data if needed.
	 */
	virtual void onShown() = 0;
	/**
	 * subclass must set the given registration delta into the registration manager.
	 */
	virtual void applyRegistration(Transform3D delta) = 0;

	/**
	 * subclass must implement to setup widget
	 */
	virtual void setup() = 0;

	virtual double getDefaultAutoLTS() const { return true; }

protected:
	virtual void prePaintEvent();

private slots:
	void obscuredSlot(bool obscured);
protected slots:
	void registerSlot();

protected:
	DoublePropertyPtr mLTSRatio;
	DoublePropertyPtr mMargin;
	BoolPropertyPtr mLinear;
	BoolPropertyPtr mAutoLTS;
	BoolPropertyPtr mDisplayProgress;
	BoolPropertyPtr mOneStep;

	ICPWidget* mICPWidget;
	XmlOptionFile mOptions;
	SeansVesselRegPtr mRegistrator;
	boost::shared_ptr<class WidgetObscuredListener> mObscuredListener;

	void onSpacesChanged();
	void onSettingsChanged();

private:
	MeshInViewPtr mMeshInView;

	void initializeProperties();
	std::vector<PropertyPtr> getAllProperties();
	void updateDifferenceLines();
	void onDisplayProgressChanged();
	void initialize();
};


}//namespace cx

#endif // CXICPREGISTRATIONBASEWIDGET_H_
