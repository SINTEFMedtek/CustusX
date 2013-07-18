#include "cxImagePropertiesWidget.h"

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

ImagePropertiesWidget::ImagePropertiesWidget(QWidget* parent) :
		TabbedWidget(parent, "ImagePropertiesWidget", "Slice Properties")
{
  this->insertWidgetAtTop(new DataSelectWidget(this, ActiveImageStringDataAdapter::New()));
  this->addTab(new VolumeInfoWidget(this), "Info");
  this->addTab(new ColorWidget(this), "Color");
  this->addTab(new OverlayWidget(this), "Overlay");
}

ImagePropertiesWidget::~ImagePropertiesWidget()
{}

QString ImagePropertiesWidget::defaultWhatsThis() const
{
  return "<html>"
    "<h3>Image slice properties.</h3>"
    "<p>Lets you set properties on a 2d image slice.</p>"
    "<p><i></i></p>"
    "</html>";
}

void ImagePropertiesWidget::showEvent(QShowEvent* event)
{
  QWidget::showEvent(event);
}

void ImagePropertiesWidget::hideEvent(QCloseEvent* event)
{
  QWidget::closeEvent(event);
}

}//end namespace cx
