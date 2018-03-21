/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#include "cxSlicePropertiesWidget.h"

#include "cxDataInterface.h"
#include "cxVolumeInfoWidget.h"
#include "cxSelectDataStringProperty.h"
#include "cxDataSelectWidget.h"
#include "cxOverlayWidget.h"
#include "cxColorWidget.h"
#include "cxTransferFunction2DOpacityWidget.h"
#include "cxTransferFunction2DColorWidget.h"
#include "cxPatientModelService.h"

namespace cx
{

SlicePropertiesWidget::SlicePropertiesWidget(PatientModelServicePtr patientModelService, ViewServicePtr viewService, QWidget* parent) :
	TabbedWidget(parent, "slice_properties_widget", "Slice Properties")
{
	this->setToolTip("2D Image properties");
	StringPropertyActiveDataPtr activeDataProperty = StringPropertyActiveData::New(patientModelService, "image|trackedStream");
	this->insertWidgetAtTop(new DataSelectWidget(viewService, patientModelService, this, activeDataProperty));
	this->addTab(new ColorWidget(patientModelService, this), "Color");
	this->addTab(new OverlayWidget(patientModelService, viewService, this), "Overlay");
//	this->addTab(new VolumeInfoWidget(patientModelService, this), "Info");//Not neccesary as this is part of Volume Properties?
}

SlicePropertiesWidget::~SlicePropertiesWidget()
{}


}//end namespace cx
