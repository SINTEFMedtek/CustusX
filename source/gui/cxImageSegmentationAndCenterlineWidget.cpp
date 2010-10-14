#include "cxImageSegmentationAndCenterlineWidget.h"

#include <QVBoxLayout>
#include <QGroupBox>
#include <QPushButton>
#include "cxSegmentationWidget.h"

namespace cx
{
//------------------------------------------------------------------------------
ImageSegmentationAndCenterlineWidget::ImageSegmentationAndCenterlineWidget(QWidget* parent) :
    WhatsThisWidget(parent)
{
  QVBoxLayout* layout = new QVBoxLayout(this);
  mSegmentationWidget = new SegmentationWidget(this);
  mCenterlineWidget =  new CenterlineWidget(this);

  layout->addWidget(this->createGroupBox(mSegmentationWidget, "Segmentation", true, true));
  layout->addWidget(this->createGroupBox(mCenterlineWidget, "Centerline"));
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

//------------------------------------------------------------------------------
RegisterWidget::RegisterWidget(QWidget* parent) :
    WhatsThisWidget(parent),
    mRegisterButton(new QPushButton("Register"))
{
  QVBoxLayout* layout = new QVBoxLayout(this);
  layout->addWidget(mRegisterButton);
  layout->addStretch();
}

RegisterWidget::~RegisterWidget()
{}

QString RegisterWidget::defaultWhatsThis() const
{
  return "<html>"
      "<h3>Registration of vessel segments to eachother.</h3>"
      "<p><i>Press the button to perform vessel based registration between image 1 and image 2s segments.</i></p>"
      "</html>";
}
//------------------------------------------------------------------------------

}//namespace cx
