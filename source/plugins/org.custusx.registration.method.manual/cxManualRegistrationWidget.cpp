/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxManualRegistrationWidget.h"
#include <boost/bind.hpp>
#include <ctkPluginContext.h>
#include "cxLabeledComboBoxWidget.h"

#include "cxRegistrationService.h"
#include "cxData.h"
#include "cxTransform3DWidget.h"

namespace cx
{

ManualImageRegistrationWidget::ManualImageRegistrationWidget(RegServicesPtr services, QWidget* parent, QString objectName, QString windowTitle) :
	BaseWidget(parent, objectName, windowTitle),
	mVerticalLayout(new QVBoxLayout(this)),
	mServices(services)
{
	this->setToolTip("Set image position directly");
	mVerticalLayout->setMargin(0);
	mLabel = new QLabel("Data matrix rMd");
	mVerticalLayout->addWidget(mLabel);
	mMatrixWidget = new Transform3DWidget(this);
	mVerticalLayout->addWidget(mMatrixWidget);
	connect(mMatrixWidget, SIGNAL(changed()), this, SLOT(matrixWidgetChanged()));
	mMatrixWidget->setEditable(true);

	mVerticalLayout->addStretch();

	connect(mServices->registration().get(), SIGNAL(movingDataChanged(QString)), this, SLOT(movingDataChanged()));
}

void ManualImageRegistrationWidget::showEvent(QShowEvent* event)
{
	this->imageMatrixChanged();
	this->movingDataChanged();
}

/** Called when the moving data in the RegistrationManager has changed.
 *  Update connections.
 */
void ManualImageRegistrationWidget::movingDataChanged()
{
	if (mConnectedMovingImage)
		disconnect(mConnectedMovingImage.get(), SIGNAL(transformChanged()), this, SLOT(imageMatrixChanged()));

	mConnectedMovingImage = mServices->registration()->getMovingData();

	if (mConnectedMovingImage)
		connect(mConnectedMovingImage.get(), SIGNAL(transformChanged()), this, SLOT(imageMatrixChanged()));

	mLabel->setText(this->getDescription());

	mMatrixWidget->setEnabled(mConnectedMovingImage!=0);
	this->imageMatrixChanged();
}

/** Called when the matrix in the widget has changed.
 *  Perform registration.
 *
 */
void ManualImageRegistrationWidget::matrixWidgetChanged()
{
	if (!mConnectedMovingImage)
		return;
	this->setMatrixFromWidget(mMatrixWidget->getMatrix());
}

/** Called when moving image has changed.
 *  Updates the displayed matrix.
 */
void ManualImageRegistrationWidget::imageMatrixChanged()
{
	mMatrixWidget->blockSignals(true);
	if (mConnectedMovingImage)
		mMatrixWidget->setMatrix(this->getMatrixFromBackend());
	else
		mMatrixWidget->setMatrix(Transform3D::Identity());
	mMatrixWidget->blockSignals(false);
}

} /* namespace cx */
