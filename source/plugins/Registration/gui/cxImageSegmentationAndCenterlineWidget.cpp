#include "cxImageSegmentationAndCenterlineWidget.h"

#include <QVBoxLayout>
#include <QPushButton>
#include <QFrame>
#include "cxBinaryThresholdImageFilterWidget.h"
#include "cxDataInterface.h"
#include "sscLabeledComboBoxWidget.h"
#include "sscTypeConversions.h"
#include "cxColorSelectButton.h"
#include "sscDataManager.h"
#include "sscMesh.h"
#include "cxRegistrationManager.h"

namespace cx
{
//------------------------------------------------------------------------------
ImageSegmentationAndCenterlineWidget::ImageSegmentationAndCenterlineWidget(RegistrationManagerPtr regManager, QWidget* parent) :
    RegistrationBaseWidget(regManager, parent, "ImageSegmentationAndCenterlineWidget", "ImageSegmentationAndCenterlineWidget")
{
  mLayout = new QVBoxLayout(this);
  mResampleWidget =  new ResampleWidget(this);
  mSegmentationWidget = new BinaryThresholdImageFilterWidget(this);
//  mSurfaceWidget = new SurfaceWidget(this);
  mCenterlineWidget =  new CenterlineWidget(this);

  mResampleOutput = SelectImageStringDataAdapter::New();
  mResampleOutput->setValueName("Output: ");
  connect(mResampleOutput.get(), SIGNAL(imageChanged(QString)), mSegmentationWidget, SLOT(setImageInputSlot(QString)));

  mSegmentationOutput = SelectImageStringDataAdapter::New();
  mSegmentationOutput->setValueName("Output: ");
  connect(mSegmentationOutput.get(), SIGNAL(imageChanged(QString)), mCenterlineWidget, SLOT(setImageInputSlot(QString)));
//  connect(mSegmentationOutput.get(), SIGNAL(imageChanged(QString)), mSurfaceWidget, SLOT(setImageInputSlot(QString)));

//  mSurfaceOutput = SelectMeshStringDataAdapter::New();
//  mSurfaceOutput->setValueName("Output: ");
  //TODO connect to view!
  //connect(surfaceOutput.get(), SIGNAL(meshChanged(QString)), this, SLOT(meshChanged(QString)));

  mCenterlineOutput = SelectDataStringDataAdapter::New();
  mCenterlineOutput->setValueName("Output: ");
  connect(mCenterlineOutput.get(), SIGNAL(dataChanged(QString)), this, SLOT(setImageSlot(QString)));

  this->setColorSlot(QColor("green"));

  ColorSelectButton* colorButton = new ColorSelectButton("Color");
  colorButton->setColor(QColor("green"));
  connect(colorButton, SIGNAL(colorChanged(QColor)), this, SLOT(setColorSlot(QColor)));
  QPushButton* fixedButton = new QPushButton("Set as Fixed");
  connect(fixedButton, SIGNAL(clicked()), this, SLOT(toFixedSlot()));
  QPushButton* movingButton = new QPushButton("Set as Moving");
  connect(movingButton, SIGNAL(clicked()), this, SLOT(toMovingSlot()));

  QLayout* buttonsLayout = new QHBoxLayout;
  buttonsLayout->addWidget(colorButton);
  buttonsLayout->addWidget(fixedButton);
  buttonsLayout->addWidget(movingButton);


  mLayout->addWidget(this->createHorizontalLine());
  mLayout->addWidget(this->createMethodWidget(mResampleWidget, new ssc::LabeledComboBoxWidget(this, mResampleOutput), "Resample"));
  mLayout->addWidget(this->createHorizontalLine());
  mLayout->addWidget(this->createMethodWidget(mSegmentationWidget, new ssc::LabeledComboBoxWidget(this, mSegmentationOutput), "Segmentation"));
  mLayout->addWidget(this->createHorizontalLine());
//  mLayout->addWidget(this->createMethodWidget(mSurfaceWidget, new ssc::LabeledComboBoxWidget(this, mSurfaceOutput), "Surface"));
//  mLayout->addWidget(this->createHorizontalLine());
  mLayout->addWidget(this->createMethodWidget(mCenterlineWidget, new ssc::LabeledComboBoxWidget(this, mCenterlineOutput), "Centerline"));
  mLayout->addWidget(this->createHorizontalLine());
  mLayout->addLayout(buttonsLayout);
  mLayout->addStretch();

  connect(mResampleWidget, SIGNAL(outputImageChanged(QString)), this , SLOT(resampleOutputArrived(QString)));
  connect(mSegmentationWidget, SIGNAL(outputImageChanged(QString)), this , SLOT(segmentationOutputArrived(QString)));
//  connect(mSurfaceWidget, SIGNAL(outputMeshChanged(QString)), this, SLOT(surfaceOutputArrived(QString)));
  connect(mCenterlineWidget, SIGNAL(outputImageChanged(QString)), this, SLOT(centerlineOutputArrived(QString)));

}

void ImageSegmentationAndCenterlineWidget::setColorSlot(QColor color)
{
  mCenterlineWidget->setDefaultColor(color);
  mSegmentationWidget->setDefaultColor(color);

  ssc::MeshPtr mesh;

  mesh = ssc::dataManager()->getMesh(mCenterlineOutput->getValue());
  if (mesh)
    mesh->setColor(color);
  mesh = ssc::dataManager()->getMesh(mSegmentationOutput->getValue());
  if (mesh)
    mesh->setColor(color);
}

void ImageSegmentationAndCenterlineWidget::toMovingSlot()
{
  ssc::DataPtr data = ssc::dataManager()->getData(mCenterlineOutput->getValue());
  if (data)
    mManager->setMovingData(data);
}

void ImageSegmentationAndCenterlineWidget::toFixedSlot()
{
  ssc::DataPtr data = ssc::dataManager()->getData(mCenterlineOutput->getValue());
  if (data)
    mManager->setFixedData(data);
}

ImageSegmentationAndCenterlineWidget::~ImageSegmentationAndCenterlineWidget()
{}

QString ImageSegmentationAndCenterlineWidget::defaultWhatsThis() const
{
  return "<html>"
      "<h3>Segmentation and centerline extraction for the i2i registration.</h3>"
      "<p><i>Segment out blood vessels from the selected image, then extract the centerline."
      "When finished, set the result as moving or fixed data in the registration tab.</i></p>"
      "<p><b>Tip:</b> The centerline extraction can take a <b>long</b> time.</p>"
      "</html>";
}


void ImageSegmentationAndCenterlineWidget::setImageSlot(QString uid)
{
}

void ImageSegmentationAndCenterlineWidget::resampleOutputArrived(QString uid)
{
  mResampleOutput->setValue(uid);
}

void ImageSegmentationAndCenterlineWidget::segmentationOutputArrived(QString uid)
{
  mSegmentationOutput->setValue(uid);
}



void ImageSegmentationAndCenterlineWidget::centerlineOutputArrived(QString uid)
{
  mCenterlineOutput->setValue(uid);
}


//------------------------------------------------------------------------------
}//namespace cx
