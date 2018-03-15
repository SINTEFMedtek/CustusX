/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxNavigationWidget.h"

#include <QGroupBox>
#include <QRadioButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QSlider>
#include <QToolBar>

#include "cxTrackingService.h"
#include "cxDoubleWidgets.h"
#include "cxDataInterface.h"
#include "cxHelperWidgets.h"
#include "cxViewService.h"
#include "cxActiveToolProxy.h"

namespace cx
{
NavigationWidget::NavigationWidget(ViewServicePtr viewService, TrackingServicePtr trackingService, QWidget* parent) :
	BaseWidget(parent, "navigation_widget", "Navigation Properties"),
    mVerticalLayout(new QVBoxLayout(this)),
    mCameraGroupBox(new QGroupBox(tr("Camera Style"), this)),
    mCameraGroupLayout(new QVBoxLayout())
{
	this->setToolTip("Camera follow style");
  //camera setttings
  mCameraGroupBox->setLayout(mCameraGroupLayout);

  QToolBar* toolBar = new QToolBar(this);
  mCameraGroupLayout->addWidget(toolBar);
  toolBar->addActions(viewService->getInteractorStyleActionGroup()->actions());

  DoublePropertyBasePtr offset = DoublePropertyActiveToolOffset::create(ActiveToolProxy::New(trackingService));
  QWidget* toolOffsetWidget = new SliderGroupWidget(this, offset);

  //layout
  this->setLayout(mVerticalLayout);
  mVerticalLayout->addWidget(mCameraGroupBox);
  mVerticalLayout->addWidget(toolOffsetWidget);
  mVerticalLayout->addStretch();
}

NavigationWidget::~NavigationWidget()
{}


}
