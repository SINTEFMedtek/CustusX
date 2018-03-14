/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxFastLandmarkPatientRegistrationWidget.h"

#include <QVBoxLayout>
#include <QPushButton>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QSlider>
#include <QGridLayout>
#include <QSpinBox>
#include <vtkDoubleArray.h>
#include "cxVector3D.h"
#include "cxLogger.h"
#include "cxLabeledComboBoxWidget.h"
#include "cxLandmarkRep.h"
#include "cxView.h"
#include "cxRegistrationService.h"
#include "cxViewService.h"
#include "cxPatientModelService.h"
#include "cxViewGroupData.h"
#include "cxReporter.h"
#include "cxLandmarkListener.h"
#include "cxActiveData.h"

namespace cx
{

FastLandmarkPatientRegistrationWidget::FastLandmarkPatientRegistrationWidget(RegServicesPtr services, QWidget* parent, QString objectName, QString windowTitle) :
	LandmarkPatientRegistrationWidget(services, parent, objectName, windowTitle)
{
}

//The following functions look (almost) exactly like the same functions in PatientLandMarksWidget
void FastLandmarkPatientRegistrationWidget::performRegistration()
{
	mServices->registration()->doFastRegistration_Translation();
	this->updateAverageAccuracyLabel();
}

}//namespace cx
