#include "cxImageSegmentationAndCenterlineWidget.h"

#include <QVBoxLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QFrame>
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

  SelectImageStringDataAdapterPtr segmentationOutput = SelectImageStringDataAdapter::New();
  segmentationOutput->setValueName("Output: ");
  connect(segmentationOutput.get(), SIGNAL(imageChanged(QString)), mCenterlineWidget, SLOT(setImageInputSlot(QString)));
  connect(segmentationOutput.get(), SIGNAL(imageChanged(QString)), mSurfaceWidget, SLOT(setImageInputSlot(QString)));

  SelectImageStringDataAdapterPtr surfaceOutput = SelectImageStringDataAdapter::New();
  surfaceOutput->setValueName("Output: ");
  //TODO connect to view!
  //connect(surfaceOutput.get(), SIGNAL(imageChanged(QString)), this, SLOT(imageChanged(QString)));

  SelectImageStringDataAdapterPtr centerlineOutput = SelectImageStringDataAdapter::New();
  centerlineOutput->setValueName("Output: ");
  connect(centerlineOutput.get(), SIGNAL(imageChanged(QString)), this, SLOT(setImageSlot(QString)));

  mLayout->addWidget(this->createHorizontalLine());
  mLayout->addWidget(this->createMethodWidget(mSegmentationWidget, new ssc::LabeledComboBoxWidget(this, segmentationOutput), "Segmentation"));
  mLayout->addWidget(this->createHorizontalLine());
  mLayout->addWidget(this->createMethodWidget(mSurfaceWidget, new ssc::LabeledComboBoxWidget(this, surfaceOutput), "Surface"));
  mLayout->addWidget(this->createHorizontalLine());
  mLayout->addWidget(this->createMethodWidget(mCenterlineWidget, new ssc::LabeledComboBoxWidget(this, centerlineOutput), "Centerline"));
  mLayout->addWidget(this->createHorizontalLine());
}

ImageSegmentationAndCenterlineWidget::~ImageSegmentationAndCenterlineWidget()
{}

void ImageSegmentationAndCenterlineWidget::setImageSlot(QString uid)
{
  if(!mOutput)
    return;
  mOutput->setValue(uid);
}


//------------------------------------------------------------------------------

FixedImage2ImageWidget::FixedImage2ImageWidget(QWidget* parent) :
    ImageSegmentationAndCenterlineWidget(parent)
{
  mOutput = RegistrationFixedImageStringDataAdapter::New();
  mLayout->addWidget(new ssc::LabeledComboBoxWidget(this, mOutput));
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
  mOutput = RegistrationMovingImageStringDataAdapter::New();
  mLayout->addWidget(new ssc::LabeledComboBoxWidget(this, mOutput));
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
