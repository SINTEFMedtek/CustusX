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

SlicePropertiesWidget::SlicePropertiesWidget(PatientModelServicePtr patientModelService, VisualizationServicePtr visualizationService, QWidget* parent) :
	TabbedWidget(parent, "SlicePropertiesWidget", "Slice Properties")
{
	this->setToolTip("2D Image properties");
	StringPropertyActiveDataPtr activeDataProperty = StringPropertyActiveData::New(patientModelService, "image|trackedStream");
	this->insertWidgetAtTop(new DataSelectWidget(visualizationService, patientModelService, this, activeDataProperty));
	this->addTab(new ColorWidget(patientModelService, this), "Color");
	this->addTab(new OverlayWidget(patientModelService, visualizationService, this), "Overlay");
	this->addTab(new VolumeInfoWidget(patientModelService, this), "Info");//Not neccesary as this is part of Volume Properties?
}

SlicePropertiesWidget::~SlicePropertiesWidget()
{}


}//end namespace cx
