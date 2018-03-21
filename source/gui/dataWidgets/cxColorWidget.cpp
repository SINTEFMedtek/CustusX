/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxColorWidget.h"
#include "cxTransferFunctionPresetWidget.h"
#include "cxTransferFunction2DColorWidget.h"
#include "cxPatientModelService.h"

namespace cx {

ColorWidget::ColorWidget(PatientModelServicePtr patientModelService, QWidget* parent) :
		BaseWidget(parent, "color_widget", "Color")
{
	this->setToolTip("Set 2D image properties");
	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->setMargin(0);
	layout->addWidget(new TransferFunction2DColorWidget(patientModelService->getActiveData(), this), 0);
	layout->addStretch(1);
	layout->addWidget(new TransferFunctionPresetWidget(patientModelService, this, false), 0);
}

} /* namespace cx */
