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

#include "cxRegistrationMethodModelToUSWidget.h"
#include <ctkPluginContext.h>
#include <QLabel>
#include <QVBoxLayout>
#include "cxRegistrationServiceProxy.h"
#include "cxPipeline.h"
#include "cxFilter.h"
//#include "cxPipelineWidget.h"

namespace cx
{

RegistrationMethodModelToUSWidget::RegistrationMethodModelToUSWidget(ctkPluginContext *pluginContext, QWidget* parent, QString objectName) :
	BaseWidget(parent, objectName, "Model to US Registration"),
	mVerticalLayout(new QVBoxLayout(this)),
	mRegistrationService(new cx::RegistrationServiceProxy(pluginContext))
{
//	mVerticalLayout->addWidget(new QLabel("Hello Plugin!"));

	//Prepare step

//	mPipeline.reset(new Pipeline(pluginContext));
//	FilterGroupPtr filters(new FilterGroup(options.descend("pipeline")));

//	filters->append(FilterPtr(new BinaryThresholdImageFilter(pluginContext)));

//	mPipeline->initialize(filters);
//	PipelineWidget* mPipelineWidget = new PipelineWidget(NULL, mPipeline);
//	mVerticalLayout->addWidget(mPipelineWidget);

}

RegistrationMethodModelToUSWidget::~RegistrationMethodModelToUSWidget()
{
}

QString RegistrationMethodModelToUSWidget::defaultWhatsThis() const
{
  return "<html>"
      "<h3>Example plugin.</h3>"
      "<p>Used for developers as a starting points for developing a new plugin</p>"
      "</html>";
}



} /* namespace cx */
