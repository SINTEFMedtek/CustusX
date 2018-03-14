/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxImagePropertiesWidget.h"

#include <QComboBox>
#include <QVBoxLayout>
#include "cxImage.h"
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
#include "cxOverlayWidget.h"
#include "cxColorWidget.h"

namespace cx
{

ImagePropertiesWidget::ImagePropertiesWidget(VisServicesPtr services, QWidget *parent) :
		TabbedWidget(parent, "ImagePropertiesWidget", "Image Properties")
{
	this->setToolTip("Image properties");
//	this->insertWidgetAtTop(new ActiveVolumeWidget(services->patient(), services->view(), this));

	bool connectToActiveImage = true;

	TabbedWidget* props2D = new TabbedWidget(NULL, "2D", "2D");
	TabbedWidget* props3D = new TabbedWidget(NULL, "3D", "3D");

	this->addTab(new VolumeInfoWidget(services->patient(), this), "Info");
	this->addTab(props2D, "2D");
	this->addTab(props3D, "3D");

	props3D->addTab(new TransferFunctionWidget(services->patient(), this, connectToActiveImage), QString("Transfer Functions"));
	props3D->addTab(new ShadingWidget(services->patient()->getActiveData(), this, connectToActiveImage), "Properties");
	props3D->addTab(new CroppingWidget(services->patient(), services->view(), this), "Crop");
	props3D->addTab(new ClippingWidget(services, this), "Clip");

	props2D->addTab(new ColorWidget(services->patient(), this), "Color");
	props2D->addTab(new OverlayWidget(services->patient(), services->view(), this), "Overlay");

	mTabWidget->setCurrentWidget(props3D);
}


}//end namespace cx
