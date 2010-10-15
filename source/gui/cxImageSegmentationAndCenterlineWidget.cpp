#include "cxImageSegmentationAndCenterlineWidget.h"

#include <QVBoxLayout>
#include <QGroupBox>
#include <QPushButton>
#include "cxSegmentationWidget.h"
#include "cxDataInterface.h"
#include "sscLabeledComboBoxWidget.h"

namespace cx
{
//------------------------------------------------------------------------------
ImageSegmentationAndCenterlineWidget::ImageSegmentationAndCenterlineWidget(QWidget* parent) :
    WhatsThisWidget(parent)
{
  mLayout = new QVBoxLayout(this);
  mSegmentationWidget = new SegmentationWidget(this);
  mSurfaceWidget = new SurfaceWidget(this);
  mCenterlineWidget =  new CenterlineWidget(this);

  SelectImageStringDataAdapterPtr startSegmentImage = SelectImageStringDataAdapter::New();
  //connect(startSegmentImage.get(), SIGNAL(imageChanged(QString)), this, SIGNAL(imageChanged(QString)));

  SelectImageStringDataAdapterPtr startCenterlineImage = SelectImageStringDataAdapter::New();
  //connect(startCenterlineImage.get(), SIGNAL(imageChanged(QString)), this, SIGNAL(imageChanged(QString)));

  mLayout->addWidget(this->createGroupBox(mSegmentationWidget, new ssc::LabeledComboBoxWidget(this, startSegmentImage), "Generate segmentation", true, false));
  mLayout->addWidget(this->createGroupBox(mSurfaceWidget, new QWidget(),"Visualize surface <optional>", true, false));
  mLayout->addWidget(this->createGroupBox(mCenterlineWidget, new ssc::LabeledComboBoxWidget(this, startSegmentImage),"Generate centerline", true, false));
}

ImageSegmentationAndCenterlineWidget::~ImageSegmentationAndCenterlineWidget()
{}

//------------------------------------------------------------------------------

FixedImage2ImageWidget::FixedImage2ImageWidget(QWidget* parent) :
    ImageSegmentationAndCenterlineWidget(parent)
{
  ssc::StringDataAdapterPtr fixedImage = RegistrationFixedImageStringDataAdapter::New();
  mLayout->addWidget(new ssc::LabeledComboBoxWidget(this, fixedImage));
  mLayout->addStretch();
};

FixedImage2ImageWidget::~FixedImage2ImageWidget()
{}

QString FixedImage2ImageWidget::defaultWhatsThis() const
{
  return "<html>"
      "<h3>Segmentation and centerline extraction for the fixed image.</h3>"
      "<p><i>Segment out blood vessels from the selected image, then extract the centerline.</i></p>"
      "<p><b>Tip:</b> The centerline extraction can take a <b>long</b> time.</p>"
      "</html>";
}
//------------------------------------------------------------------------------

MovingImage2ImageWidget::MovingImage2ImageWidget(QWidget* parent) :
    ImageSegmentationAndCenterlineWidget(parent)
{
  ssc::StringDataAdapterPtr movingImage = RegistrationMovingImageStringDataAdapter::New();
  mLayout->addWidget(new ssc::LabeledComboBoxWidget(this, movingImage));
  mLayout->addStretch();
};

MovingImage2ImageWidget::~MovingImage2ImageWidget()
{}

QString MovingImage2ImageWidget::defaultWhatsThis() const
{
  return "<html>"
      "<h3>Segmentation and centerline extraction for the moving image.</h3>"
      "<p><i>Segment out blood vessels from the selected image, then extract the centerline.</i></p>"
      "<p><b>Tip:</b> The centerline extraction can take a <b>long</b> time.</p>"
      "</html>";
}
//------------------------------------------------------------------------------
}//namespace cx
