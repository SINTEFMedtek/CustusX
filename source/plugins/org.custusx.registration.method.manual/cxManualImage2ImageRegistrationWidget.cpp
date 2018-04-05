/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxManualImage2ImageRegistrationWidget.h"
#include "cxRegistrationService.h"
#include "cxData.h"
#include "cxImage.h"
#include "cxRegistrationProperties.h"
#include "cxLabeledComboBoxWidget.h"
#include "cxPatientModelService.h"
#include "cxRegistrationTransform.h"
#include "cxLandmark.h"

#include "vtkMath.h"

namespace cx
{


ManualImage2ImageRegistrationWidget::ManualImage2ImageRegistrationWidget(RegServicesPtr services, QWidget *parent, QString objectName) :
	ManualImageRegistrationWidget(services, parent, objectName, "Manual Image to Image Registration")
{
	StringPropertyBasePtr fixedImage(new StringPropertyRegistrationFixedImage(services->registration(), services->patient()));
	StringPropertyBasePtr movingImage(new StringPropertyRegistrationMovingImage(services->registration(), services->patient()));

	LabeledComboBoxWidget* fixed = new LabeledComboBoxWidget(this, fixedImage);
	LabeledComboBoxWidget* moving = new LabeledComboBoxWidget(this, movingImage);

    mAvarageAccuracyLabel = new QLabel(QString(" "), this);

	mVerticalLayout->insertWidget(0, fixed);
	mVerticalLayout->insertWidget(1, moving);
    mVerticalLayout->insertWidget(2, mAvarageAccuracyLabel);
}

QString ManualImage2ImageRegistrationWidget::getDescription()
{
	if (this->isValid())
		return QString("<b>Matrix fMm from moving to fixed image</b>");
	else
		return "<Invalid matrix>";
}

bool ManualImage2ImageRegistrationWidget::isValid() const
{
	return mServices->registration()->getMovingData() && mServices->registration()->getFixedData();
}

Transform3D ManualImage2ImageRegistrationWidget::getMatrixFromBackend()
{
    if (!this->isValid())               // Check if fixed and moving data are defined
		return Transform3D::Identity();

	Transform3D rMm = mServices->registration()->getMovingData()->get_rMd();
	Transform3D rMf = mServices->registration()->getFixedData()->get_rMd();
	Transform3D fMm = rMf.inv() * rMm;

    RegistrationHistoryPtr history = mServices->registration()->getMovingData()->get_rMd_History();
    Transform3D init_rMd;
    if(!history->getData().empty())     // Is vector with RegistrationTransforms empty ?
        init_rMd = history->getData().front().mValue;
    else
        init_rMd = Transform3D::Identity();

    Transform3D current_rMd = history->getCurrentRegistration().mValue;
    fMm = current_rMd * init_rMd.inv();

    return fMm;
}

void ManualImage2ImageRegistrationWidget::setMatrixFromWidget(Transform3D M)
{
    if (!this->isValid())               // Check if fixed and moving data are defined
		return;

	Transform3D rMm = mServices->registration()->getMovingData()->get_rMd();

    RegistrationHistoryPtr history = mServices->registration()->getMovingData()->get_rMd_History();

    Transform3D init_rMd;

    if(!history->getData().empty())     // Is vector with RegistrationTransforms empty ?
        init_rMd = history->getData().front().mValue;
    else
        init_rMd = Transform3D::Identity();

    Transform3D new_rMd = M * init_rMd;
    Transform3D delta = new_rMd * rMm.inv();

    mServices->registration()->addImage2ImageRegistration(delta, "Manual Image");
    this->updateAverageAccuracyLabel();

}

double ManualImage2ImageRegistrationWidget::getAverageAccuracy(int& numActiveLandmarks)
{
    std::map<QString, LandmarkProperty> props = mServices->patient()->getLandmarkProperties();

    double sum = 0;
    numActiveLandmarks = 0;
    std::map<QString, LandmarkProperty>::iterator it = props.begin();
    for (; it != props.end(); ++it)
    {
        if (!it->second.getActive()) //we don't want to take into account not active landmarks
            continue;
        QString uid = it->first;
        double val = this->getAccuracy(uid);
        if (!similar(val, 1000.0))
        {
            sum = sum + val;
            numActiveLandmarks++;
        }
    }
    if (numActiveLandmarks == 0)
        return 1000.0;
    return (sqrt(sum / (double)numActiveLandmarks));
}

double ManualImage2ImageRegistrationWidget::getAccuracy(QString uid)
{
    DataPtr fixedData = mServices->registration()->getFixedData();
    if (!fixedData)
        return 1000.0;
    DataPtr movingData = mServices->registration()->getMovingData();
    if (!movingData)
        return 1000.0;

    Landmark masterLandmark = fixedData->getLandmarks()->getLandmarks()[uid];
    Landmark targetLandmark = movingData->getLandmarks()->getLandmarks()[uid];
    if (masterLandmark.getUid().isEmpty() || targetLandmark.getUid().isEmpty())
        return 1000.0;

    Vector3D p_master_master = masterLandmark.getCoord();
    Vector3D p_target_target = targetLandmark.getCoord();
    Transform3D rMmaster = fixedData->get_rMd();
    Transform3D rMtarget = movingData->get_rMd();

    Vector3D p_target_r = rMtarget.coord(p_target_target);
    Vector3D p_master_r = rMmaster.coord(p_master_master);

    double  targetPoint[3];
    double  masterPoint[3];
    targetPoint[0] = p_target_r[0];
    targetPoint[1] = p_target_r[1];
    targetPoint[2] = p_target_r[2];
    masterPoint[0] = p_master_r[0];
    masterPoint[1] = p_master_r[1];
    masterPoint[2] = p_master_r[2];

    return (vtkMath::Distance2BetweenPoints(targetPoint, masterPoint));
}

void    ManualImage2ImageRegistrationWidget::updateAverageAccuracyLabel()
{
    QString fixedName;
    QString movingName;
    DataPtr fixedData = boost::dynamic_pointer_cast<Data>(mServices->registration()->getFixedData());
    DataPtr movingData = boost::dynamic_pointer_cast<Data>(mServices->registration()->getMovingData());
    if (fixedData)
        fixedName = fixedData->getName();
    if (movingData)
        movingName = movingData->getName();

    int numberOfActiveLandmarks = 0;
    if(this->isAverageAccuracyValid(numberOfActiveLandmarks))
    {
        mAvarageAccuracyLabel->setText(tr("Root mean square accuracy (Landmarks) %1 mm, calculated in %2 landmarks").
                                       arg(this->getAverageAccuracy(numberOfActiveLandmarks), 0, 'f', 2).arg(numberOfActiveLandmarks));
        mAvarageAccuracyLabel->setToolTip(QString("Root Mean Square landmark accuracy from target [%1] to fixed [%2].").
                                          arg(movingName).arg(fixedName));
    }
    else
    {
        mAvarageAccuracyLabel->setText(" ");
        mAvarageAccuracyLabel->setToolTip("");
    }
}

bool    ManualImage2ImageRegistrationWidget::isAverageAccuracyValid(int& numberOfActiveLandmarks)
{
    int numActiveLandmarks = 0;
    this->getAverageAccuracy(numActiveLandmarks);
    if(numActiveLandmarks < 1)
        return false;
    return true;
}


} // cx
