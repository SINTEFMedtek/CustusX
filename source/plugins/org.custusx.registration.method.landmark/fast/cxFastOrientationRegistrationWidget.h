/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXFASTORIENTATIONREGISTRATIONWIDGET_H_
#define CXFASTORIENTATIONREGISTRATIONWIDGET_H_

#include "cxRegistrationBaseWidget.h"
#include "cxForwardDeclarations.h"
#include "cxTransform3D.h"
#include "cxActiveToolProxy.h"

class QPushButton;
class QCheckBox;

namespace cx
{
/**
 * \file
 * \addtogroup org_custusx_registration_method_landmark
 * @{
 */

/**
 * \class FastOrientationRegistrationWidget
 *
 * \brief Widget for registrating the orientation part of a fast registration
 *
 * \date 27. sep. 2010
 * \\author Janne Beate Bakeng
 */

class FastOrientationRegistrationWidget : public RegistrationBaseWidget
{
	Q_OBJECT

public:
	FastOrientationRegistrationWidget(RegServicesPtr services, QWidget* parent);
	~FastOrientationRegistrationWidget();

protected:
	virtual void showEvent(QShowEvent* event);
	virtual void hideEvent(QHideEvent* event);

private slots:
	void setOrientationSlot();
	void enableToolSampleButtonSlot();
	void globalConfigurationFileChangedSlot(QString key);
private:
	Transform3D get_tMtm() const;
	QPushButton* mSetOrientationButton;
	QCheckBox* mInvertButton;
	ActiveToolProxyPtr mActiveToolProxy;
};

/**
 * @}
 */
}
#endif /* CXFASTORIENTATIONREGISTRATIONWIDGET_H_ */
