// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.

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
