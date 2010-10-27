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

  mSegmentationOutput = SelectImageStringDataAdapter::New();
  mSegmentationOutput->setValueName("Output: ");
  connect(mSegmentationOutput.get(), SIGNAL(imageChanged(QString)), mCenterlineWidget, SLOT(setImageInputSlot(QString)));
  connect(mSegmentationOutput.get(), SIGNAL(imageChanged(QString)), mSurfaceWidget, SLOT(setImageInputSlot(QString)));

  mSurfaceOutput = SelectMeshStringDataAdapter::New();
  mSurfaceOutput->setValueName("Output: ");
  //TODO connect to view!
  //connect(surfaceOutput.get(), SIGNAL(meshChanged(QString)), this, SLOT(meshChanged(QString)));

  mCenterlineOutput = SelectImageStringDataAdapter::New();
  mCenterlineOutput->setValueName("Output: ");
  connect(mCenterlineOutput.get(), SIGNAL(imageChanged(QString)), this, SLOT(setImageSlot(QString)));

  mLayout->addWidget(this->createHorizontalLine());
  mLayout->addWidget(this->createMethodWidget(mSegmentationWidget, new ssc::LabeledComboBoxWidget(this, mSegmentationOutput), "Segmentation"));
  mLayout->addWidget(this->createHorizontalLine());
  mLayout->addWidget(this->createMethodWidget(mSurfaceWidget, new ssc::LabeledComboBoxWidget(this, mSurfaceOutput), "Surface"));
  mLayout->addWidget(this->createHorizontalLine());
  mLayout->addWidget(this->createMethodWidget(mCenterlineWidget, new ssc::LabeledComboBoxWidget(this, mCenterlineOutput), "Centerline"));
  mLayout->addWidget(this->createHorizontalLine());

  connect(mSegmentationWidget, SIGNAL(outputImageChanged(QString)), this , SLOT(segmentationOutputArrived(QString)));
  connect(mSurfaceWidget, SIGNAL(outputMeshChanged(QString)), this, SLOT(surfaceOutputArrived(QString)));
  connect(mCenterlineWidget, SIGNAL(outputImageChanged(QString)), this, SLOT(centerlineOutputArrived(QString)));
}

ImageSegmentationAndCenterlineWidget::~ImageSegmentationAndCenterlineWidget()
{}

void ImageSegmentationAndCenterlineWidget::setImageSlot(QString uid)
{
  if(!mOutput)
    return;
  mOutput->setValue(uid);
}

void ImageSegmentationAndCenterlineWidget::segmentationOutputArrived(QString uid)
{
  mSegmentationOutput->setValue(uid);
}

void ImageSegmentationAndCenterlineWidget::surfaceOutputArrived(QString uid)
{
  mSurfaceOutput->setValue(uid);
}

void ImageSegmentationAndCenterlineWidget::centerlineOutputArrived(QString uid)
{
  mCenterlineOutput->setValue(uid);
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
