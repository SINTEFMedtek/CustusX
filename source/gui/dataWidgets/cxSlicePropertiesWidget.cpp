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
#include "cxSlicePropertiesWidget.h"

#include "cxDataInterface.h"
#include "cxVolumeInfoWidget.h"
#include "cxSelectDataStringDataAdapter.h"
#include "cxDataSelectWidget.h"
#include "cxOverlayWidget.h"
#include "cxColorWidget.h"
#include "cxTransferFunction2DOpacityWidget.h"
#include "cxTransferFunction2DColorWidget.h"

namespace cx
{

SlicePropertiesWidget::SlicePropertiesWidget(QWidget* parent) :
		TabbedWidget(parent, "SlicePropertiesWidget", "Slice Properties")
{
  this->insertWidgetAtTop(new DataSelectWidget(this, ActiveImageStringDataAdapter::New()));
  this->addTab(new VolumeInfoWidget(this), "Info");
  this->addTab(new ColorWidget(this), "Color");
  this->addTab(new OverlayWidget(this), "Overlay");
}

SlicePropertiesWidget::~SlicePropertiesWidget()
{}

QString SlicePropertiesWidget::defaultWhatsThis() const
{
  return "<html>"
    "<h3>Image slice properties.</h3>"
    "<p>Lets you set properties on a 2d image slice.</p>"
    "<p><i></i></p>"
    "</html>";
}

void SlicePropertiesWidget::showEvent(QShowEvent* event)
{
  QWidget::showEvent(event);
}

void SlicePropertiesWidget::hideEvent(QCloseEvent* event)
{
  QWidget::closeEvent(event);
}

}//end namespace cx
