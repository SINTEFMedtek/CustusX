/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxOverlayWidget.h"
#include "cxDataViewSelectionWidget.h"
#include "cxTransferFunction2DOpacityWidget.h"
#include "cxDataSelectWidget.h"
#include "cxSelectDataStringProperty.h"
#include "cxPatientModelService.h"

namespace cx {

OverlayWidget::OverlayWidget(PatientModelServicePtr patientModelService, ViewServicePtr viewService, QWidget* parent) :
		BaseWidget(parent, "overlay_widget", "Overlay")
{
	this->setToolTip("Manage multiple overlayed 2D slices");
	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->setMargin(0);
	layout->addWidget(new TransferFunction2DOpacityWidget(patientModelService->getActiveData(), this), 0);
	layout->addWidget(new DataViewSelectionWidget(patientModelService, viewService, this), 1);
}

} /* namespace cx */
