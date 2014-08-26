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

#include "cxManualRegistrationWidget.h"
#include "cxLabeledComboBoxWidget.h"
#include "cxRegistrationDataAdapters.h"
#include "cxRegistrationManager.h"
#include "cxToolManager.h"
#include "cxDataManager.h"

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

	mConnectedMovingImage = mManager->getMovingData();

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

QString ManualImageRegistrationWidget::defaultWhatsThis() const
{
	return QString();
}

// --------------------------------------------------------
// --------------------------------------------------------
// --------------------------------------------------------

QString ManualImage2ImageRegistrationWidget::getDescription()
{
	if (this->isValid())
		return QString("<b>Matrix fMm from moving to fixed image</b>");
	else
		return "<Invalid matrix>";
}

bool ManualImage2ImageRegistrationWidget::isValid() const
{
	return mManager->getMovingData() && mManager->getFixedData();
}

Transform3D ManualImage2ImageRegistrationWidget::getMatrixFromBackend()
{
	if (!this->isValid())
		return Transform3D::Identity();

	Transform3D rMm = mManager->getMovingData()->get_rMd();
	Transform3D rMf = mManager->getFixedData()->get_rMd();
	Transform3D fMm = rMf.inv() * rMm;
	return fMm;
}

void ManualImage2ImageRegistrationWidget::setMatrixFromWidget(Transform3D M)
{
	if (!this->isValid())
		return;

	Transform3D rMm = mManager->getMovingData()->get_rMd();
	Transform3D rMf = mManager->getFixedData()->get_rMd();
	Transform3D fQm = M; // the modified fMm matrix

	// start with
	//   new registration = new registration
	//                fQm = fMr * rMm'
	//                fQm = fMr * delta * rMm
	Transform3D delta = rMf * fQm * rMm.inv();

	mManager->applyImage2ImageRegistration(delta, "Manual Image");
}

// --------------------------------------------------------
// --------------------------------------------------------
// --------------------------------------------------------

QString ManualImageTransformRegistrationWidget::getDescription()
{
	if (mConnectedMovingImage)
		return QString("<b>Position Matrix rMd for data %1</b>").arg(mConnectedMovingImage->getName());
	else
		return "<Invalid matrix>";
}

Transform3D ManualImageTransformRegistrationWidget::getMatrixFromBackend()
{
	return mConnectedMovingImage->get_rMd();
}

void ManualImageTransformRegistrationWidget::setMatrixFromWidget(Transform3D M)
{
	Transform3D rMd = mConnectedMovingImage->get_rMd();
	Transform3D rMMd = M;

	Transform3D delta_pre_rMd = rMMd * rMd.inv(); // gives delta on the r (left) side.
	mManager->applyImage2ImageRegistration(delta_pre_rMd, "Manual Image rMd");
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
