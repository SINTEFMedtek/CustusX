/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include <cxActiveToolWidget.h>

#include <QVBoxLayout>
#include "cxLabeledComboBoxWidget.h"
#include "cxToolProperty.h"

namespace cx
{

ActiveToolWidget::ActiveToolWidget(TrackingServicePtr trackingService, QWidget* parent) :
	BaseWidget(parent, "active_tool_widget", "Active Tool")
{
	this->setToolTip("Select the active tool");
  QVBoxLayout* layout = new QVBoxLayout(this);
  this->setObjectName("active_tool_widget");
  layout->setMargin(0);

  mSelector = StringPropertyActiveTool::New(trackingService);

  LabeledComboBoxWidget*  combo = new LabeledComboBoxWidget(this, mSelector);
  layout->addWidget(combo);
}

}
