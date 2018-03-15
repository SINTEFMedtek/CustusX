/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxExampleWidget.h"
#include <QLabel>
#include <QVBoxLayout>

namespace cx
{

ExampleWidget::ExampleWidget(QWidget* parent) :
    QWidget(parent),
    mVerticalLayout(new QVBoxLayout(this))
{
    this->setObjectName("ExampleWidget");
    this->setWindowTitle("Example");
    this->setWhatsThis(this->defaultWhatsThis());

    mVerticalLayout->addWidget(new QLabel("Hello Plugin!"));
}

ExampleWidget::~ExampleWidget()
{
}

QString ExampleWidget::defaultWhatsThis() const
{
  return "<html>"
      "<h3>Example plugin.</h3>"
      "<p>Used for developers as a starting points for developing a new plugin</p>"
      "</html>";
}



} /* namespace cx */
