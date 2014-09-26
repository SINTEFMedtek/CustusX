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

#include "cxRegistrationMethodToUSPrepareWidget.h"
#include "cxPipeline.h"
#include "cxFilter.h"
#include "cxBinaryThresholdImageFilter.h"
#include "cxPipelineWidget.h"

//TODO: remove
#include "cxDataLocations.h"

namespace cx {

RegistrationMethodToUSPrepareWidget::RegistrationMethodToUSPrepareWidget(VisualizationServicePtr visualizationService, PatientModelServicePtr patientModelService, QWidget* parent, QString objectName) :
	BaseWidget(parent, objectName, "Model to US Registration preparation"),
	mVerticalLayout(new QVBoxLayout(this))
{
	//Copied from PrepareVesselsWidget
	XmlOptionFile options = XmlOptionFile(DataLocations::getXmlSettingsFile(), "CustusX").descend("registration").descend("PrepareVesselsWidget");
	// fill the pipeline with filters:
	PipelinePtr mPipeline;
	mPipeline.reset(new Pipeline(patientModelService));
	FilterGroupPtr filters(new FilterGroup(options.descend("pipeline")));
	//filters->append(FilterPtr(new ResampleImageFilter(patientModelService)));
	//filters->append(FilterPtr(new SmoothingImageFilter(patientModelService)));
	filters->append(FilterPtr(new BinaryThresholdImageFilter(patientModelService)));
	//filters->append(FilterPtr(new BinaryThinningImageFilter3DFilter(patientModelService)));
	mPipeline->initialize(filters);

	PipelineWidget *mPipelineWidget = new PipelineWidget(visualizationService, patientModelService, NULL, mPipeline);
	mVerticalLayout->addWidget(mPipelineWidget);
}


QString RegistrationMethodToUSPrepareWidget::defaultWhatsThis() const
{
  return "<html>"
	  "<h3>Prepare Model to US registration.</h3>"
	  "<p>Create vessel segmentation</p>"
	  "</html>";
}

} //cx
