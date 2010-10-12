#include "cxSegmentationWidget.h"

namespace cx
{
//------------------------------------------------------------------------------
SegmentationWidget::SegmentationWidget(QWidget* parent) :
    WhatsThisWidget(parent)
{
  this->setObjectName("SegmentationWidget");
  this->setWindowTitle("Segmentation");
}

SegmentationWidget::~SegmentationWidget()
{}

QString SegmentationWidget::defaultWhatsThis() const
{
  return "<html>"
        "<h3>Segmentation.</h3>"
        "<p><i>Segment out blood vessels(?) from the selected image.</i></p>"
        "</html>";
}
//------------------------------------------------------------------------------
CenterlineWidget::CenterlineWidget(QWidget* parent) :
    WhatsThisWidget(parent)
{
  this->setObjectName("CenterlineWidget");
  this->setWindowTitle("Centerline");
}

CenterlineWidget::~CenterlineWidget()
{}

QString CenterlineWidget::defaultWhatsThis() const
{
  return "<html>"
        "<h3>Centerline extraction.</h3>"
        "<p><i>Extract the centerline from a segment.</i></p>"
        "<p><b>Tip:</b> The centerline extraction can take a <b>long</b> time.</p>"
        "</html>";
}
}//namespace cx
