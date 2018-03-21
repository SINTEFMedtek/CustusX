/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXMANUALIMAGE2IMAGEREGISTRATIONWIDGET_H
#define CXMANUALIMAGE2IMAGEREGISTRATIONWIDGET_H

#include "org_custusx_registration_method_manual_Export.h"
#include "cxManualRegistrationWidget.h"

class QLabel;

namespace cx
{

/** Direct setting of image registration
 *
 * The matrix is the fMm transform, i.e. from moving to fixed image
 *
 *  \ingroup org_custusx_registration_method_manual
 *  \date 2014-06-13
 *  \author Christian Askeland, SINTEF
 *  \author Ole Vegard Solberg, SINTEF
 *  \author Geir Arne Tangen, SINTEF
 */

class org_custusx_registration_method_manual_EXPORT ManualImage2ImageRegistrationWidget : public ManualImageRegistrationWidget
{
public:
	ManualImage2ImageRegistrationWidget(RegServicesPtr services, QWidget* parent, QString objectName);
	virtual QString getDescription();
	virtual Transform3D getMatrixFromBackend();
	virtual void setMatrixFromWidget(Transform3D M);

    bool    isValid() const;
    double  getAverageAccuracy(int& numActiveLandmarks);

private:
    QLabel* mAvarageAccuracyLabel;

    void    updateAverageAccuracyLabel();
    double  getAccuracy(QString uid);
    bool    isAverageAccuracyValid(int& numberOfActiveLandmarks);

};

} //cx

#endif // CXMANUALIMAGE2IMAGEREGISTRATIONWIDGET_H
