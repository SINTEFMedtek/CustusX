/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxVolumePropertiesWidget.h"

#include <QComboBox>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include "cxLabeledComboBoxWidget.h"
#include "cxLabeledLineEditWidget.h"
#include "cxImage.h"
#include "cxTransferFunctionWidget.h"
#include "cxCroppingWidget.h"
#include "cxClippingWidget.h"
#include "cxShadingWidget.h"
#include "cxDataInterface.h"
#include "cxDataSelectWidget.h"
#include "cxSelectDataStringProperty.h"
#include "cxVolumeInfoWidget.h"
#include "cxVolumeHelpers.h"
#include "cxTypeConversions.h"
#include "cxPatientModelService.h"
#include "cxVisServices.h"
#include "cxSelectClippersForDataWidget.h"

namespace cx
{

/// -------------------------------------------------------
/// -------------------------------------------------------
/// -------------------------------------------------------

ActiveVolumeWidget::ActiveVolumeWidget(PatientModelServicePtr patientModelService, ViewServicePtr viewService, QWidget* parent) :
  BaseWidget(parent, "active_volume_widget", "Active Volume")
{
	this->setToolTip("The currently selected active volume");
  QVBoxLayout* layout = new QVBoxLayout(this);
  layout->setMargin(0);
	layout->addWidget(new DataSelectWidget(viewService, patientModelService, this, StringPropertyActiveImage::New(patientModelService)));
}

/// -------------------------------------------------------
/// -------------------------------------------------------
/// -------------------------------------------------------

VolumePropertiesWidget::VolumePropertiesWidget(VisServicesPtr services, QWidget *parent) :
		TabbedWidget(parent, "volume_properties_widget", "Volume Properties")
{
	this->setToolTip("Volume properties");
	this->insertWidgetAtTop(new ActiveVolumeWidget(services->patient(), services->view(), this));

	bool connectToActiveImage = true;

	this->addTab(new VolumeInfoWidget(services->patient(), this), "Info");
	this->addTab(new TransferFunctionWidget(services->patient(), this, connectToActiveImage), QString("Transfer Functions"));
	this->addTab(new ShadingWidget(services->patient()->getActiveData(), this, connectToActiveImage), "Properties");
	this->addTab(new CroppingWidget(services->patient(), services->view(), this), "Crop");
	this->addTab(new SelectClippersForImageWidget(services, this), "Clip");
}

}//namespace
