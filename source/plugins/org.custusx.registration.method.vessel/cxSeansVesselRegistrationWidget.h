/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXSEANSVESSELREGISTRATIONWIDGET_H_
#define CXSEANSVESSELREGISTRATIONWIDGET_H_

#include "cxRegistrationBaseWidget.h"
#include "cxStringPropertyBase.h"

class QSpinBox;
class QPushButton;
class QLabel;
#include "cxBoolProperty.h"
#include "cxDoubleProperty.h"
#include "cxICPRegistrationBaseWidget.h"
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
 * \brief I2I (image2image) ICP registration
 *
 * \ingroup org_custusx_registration_method_vessel
 * \date Feb 21, 2011
 * \author Janne Beate Bakeng, SINTEF
 */
class org_custusx_registration_method_vessel_EXPORT SeansVesselRegistrationWidget : public ICPRegistrationBaseWidget
{
	Q_OBJECT
public:
	SeansVesselRegistrationWidget(RegServicesPtr services, QWidget* parent);
	virtual ~SeansVesselRegistrationWidget();

protected:
	virtual void initializeRegistrator();
	virtual void inputChanged();
	virtual void applyRegistration(Transform3D delta);
	virtual void onShown();
	virtual void setup();

private:
	StringPropertyBasePtr mFixedImage;
	StringPropertyBasePtr mMovingImage;

	SpaceListenerPtr mSpaceListenerMoving;
	SpaceListenerPtr mSpaceListenerFixed;
};


}//namespace cx

#endif /* CXSEANSVESSELREGISTRATIONWIDGET_H_ */
