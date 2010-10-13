#include "cxImageSegmentationAndCenterlineWidget.h"

#include <QVBoxLayout>
#include "cxSegmentationWidget.h"
//#include "cxCenterlineWidget.h"

namespace cx
{
ImageSegmentationAndCenterlineWidget::ImageSegmentationAndCenterlineWidget(QWidget* parent) :
    WhatsThisWidget(parent)
{
  QVBoxLayout* layout = new QVBoxLayout(this);
  mSegmentationWidget = new SegmentationWidget(this);
  mCenterlineWidget =  new CenterlineWidget(this);
  layout->addWidget(mSegmentationWidget);
  layout->addWidget(mCenterlineWidget);
  layout->addStretch();
}

ImageSegmentationAndCenterlineWidget::~ImageSegmentationAndCenterlineWidget()
{}

QString ImageSegmentationAndCenterlineWidget::defaultWhatsThis() const
{
  return "<html>"
      "<h3>Segmentation and centerline extraction.</h3>"
      "<p><i>Segment out blood vessels from the selected image, then extract the centerline.</i></p>"
      "<p><b>Tip:</b> The centerline extraction can take a <b>long</b> time.</p>"
      "</html>";
}

}//namespace cx
