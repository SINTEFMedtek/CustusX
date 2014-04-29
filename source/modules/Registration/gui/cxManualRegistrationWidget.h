// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.

#ifndef CXMANUALREGISTRATIONWIDGET_H_
#define CXMANUALREGISTRATIONWIDGET_H_

#include "cxRegistrationBaseWidget.h"
#include "cxStringDataAdapter.h"
#include "cxTransform3DWidget.h"
#include "cxData.h"

namespace cx
{
/**
 * \file
 * \addtogroup cx_module_registration
 * @{
 */

/**\brief Direct setting of image registration
 *
 * Manipulate the image matrix rMd via a matrix
 * or xyz+angles. The writing of the matrix is handled
 * exactly like an image registration.
 *
 *  \date Feb 16, 2012
 *  \author christiana
 */
class ManualImageRegistrationWidget: public RegistrationBaseWidget
{
Q_OBJECT
public:
	ManualImageRegistrationWidget(RegistrationManagerPtr regManager, QWidget* parent);
	virtual ~ManualImageRegistrationWidget()
	{
	}
	virtual QString defaultWhatsThis() const;

private slots:
	void matrixWidgetChanged();
	void imageMatrixChanged();
	void movingDataChanged();

private:
	QVBoxLayout* mVerticalLayout;
	QLabel* mLabel;

	StringDataAdapterPtr mFixedImage;
	StringDataAdapterPtr mMovingImage;
	Transform3DWidget* mMatrixWidget;
	DataPtr mConnectedMovingImage;

};

/**\brief Direct setting of patient registration
 *
 * Manipulate the image matrix rMpr via a matrix
 * or xyz+angles. The writing of the matrix is handled
 * exactly like an image registration.
 *
 *  \date Feb 16, 2012
 *  \author christiana
 */
class ManualPatientRegistrationWidget: public RegistrationBaseWidget
{
Q_OBJECT
public:
ManualPatientRegistrationWidget(RegistrationManagerPtr regManager, QWidget* parent);
	virtual ~ManualPatientRegistrationWidget()
	{
	}
	virtual QString defaultWhatsThis() const;

private slots:
	void matrixWidgetChanged();
	void patientMatrixChanged();

private:
	QVBoxLayout* mVerticalLayout;
	Transform3DWidget* mMatrixWidget;
};

/**
 * @}
 */
} /* namespace cx */
#endif /* CXMANUALREGISTRATIONWIDGET_H_ */
