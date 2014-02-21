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

#include "cxManualRegistrationWidget.h"
#include "sscLabeledComboBoxWidget.h"
#include "cxRegistrationDataAdapters.h"
#include "cxRegistrationManager.h"
#include "sscToolManager.h"
#include "sscDataManager.h"

namespace cx
{

ManualImageRegistrationWidget::ManualImageRegistrationWidget(RegistrationManagerPtr regManager, QWidget* parent) :
				RegistrationBaseWidget(regManager, parent, "ManualImageRegistrationWidget",
								"Manual Image Registration"), mVerticalLayout(new QVBoxLayout(this))
{
	mFixedImage.reset(new RegistrationFixedImageStringDataAdapter(regManager));
	mVerticalLayout->addWidget(new LabeledComboBoxWidget(this, mFixedImage));
	mMovingImage.reset(new RegistrationMovingImageStringDataAdapter(regManager));
	mVerticalLayout->addWidget(new LabeledComboBoxWidget(this, mMovingImage));

	mLabel = new QLabel("Data matrix rMd");
	mVerticalLayout->addWidget(mLabel);
	mMatrixWidget = new Transform3DWidget(this);
	mVerticalLayout->addWidget(mMatrixWidget);
	connect(mMatrixWidget, SIGNAL(changed()), this, SLOT(matrixWidgetChanged()));
	mMatrixWidget->setEditable(true);

	mVerticalLayout->addStretch();

	connect(mManager.get(), SIGNAL(movingDataChanged(QString)), this, SLOT(movingDataChanged()));

	this->imageMatrixChanged();
	this->movingDataChanged();
}

/**Called when the moving data in the RegistrationManager has changed.
 * Update connections.
 */
void ManualImageRegistrationWidget::movingDataChanged()
{
	if (mConnectedMovingImage)
		disconnect(mConnectedMovingImage.get(), SIGNAL(transformChanged()), this, SLOT(imageMatrixChanged()));

	mConnectedMovingImage = mManager->getMovingData();

	if (mConnectedMovingImage)
		connect(mConnectedMovingImage.get(), SIGNAL(transformChanged()), this, SLOT(imageMatrixChanged()));

	if (mConnectedMovingImage)
		mLabel->setText(QString("<b>Position Matrix rMd for data %1</b>").arg(mConnectedMovingImage->getName()));
	else
		mLabel->setText("Data matrix rMd");

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

	Transform3D rMd = mConnectedMovingImage->get_rMd();
	Transform3D rMMd = mMatrixWidget->getMatrix();

	Transform3D delta_pre_rMd = rMMd * rMd.inv(); // gives delta on the r (left) side.
	mManager->applyImage2ImageRegistration(delta_pre_rMd, "Manual Image");
}

/**Called when moving image has changed.
 * Updates the displayed matrix.
 */
void ManualImageRegistrationWidget::imageMatrixChanged()
{
	mMatrixWidget->blockSignals(true);
	if (mConnectedMovingImage)
	{
		mMatrixWidget->setMatrix(mConnectedMovingImage->get_rMd());
	}
	else
	{
		mMatrixWidget->setMatrix(Transform3D::Identity());
	}
	mMatrixWidget->blockSignals(false);
}

QString ManualImageRegistrationWidget::defaultWhatsThis() const
{
	return QString();
}

// --------------------------------------------------------
// --------------------------------------------------------
// --------------------------------------------------------

ManualPatientRegistrationWidget::ManualPatientRegistrationWidget(RegistrationManagerPtr regManager, QWidget* parent) :
				RegistrationBaseWidget(regManager, parent, "ManualPatientRegistrationWidget",
								"Manual Patient Registration"), mVerticalLayout(new QVBoxLayout(this))
{
	mVerticalLayout->addWidget(new QLabel("<b>Patient Registration matrix rMpr</b>"));
	mMatrixWidget = new Transform3DWidget(this);
	mVerticalLayout->addWidget(mMatrixWidget);
	connect(mMatrixWidget, SIGNAL(changed()), this, SLOT(matrixWidgetChanged()));
	mMatrixWidget->setEditable(true);

	mVerticalLayout->addStretch();

	connect(dataManager(), SIGNAL(rMprChanged()), this, SLOT(patientMatrixChanged()));

	this->patientMatrixChanged();
}

/** Called when the matrix in the widget has changed.
 *  Perform registration.
 *
 */
void ManualPatientRegistrationWidget::matrixWidgetChanged()
{
	Transform3D rMpr = mMatrixWidget->getMatrix();
	mManager->applyPatientRegistration(rMpr, "Manual Patient");
}

/**Called when moving image has changed.
 * Updates the displayed matrix.
 */
void ManualPatientRegistrationWidget::patientMatrixChanged()
{
	mMatrixWidget->blockSignals(true);
	mMatrixWidget->setMatrix(dataManager()->get_rMpr());
	mMatrixWidget->blockSignals(false);
}

QString ManualPatientRegistrationWidget::defaultWhatsThis() const
{
	return QString();
}


} /* namespace cx */
