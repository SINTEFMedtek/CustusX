#include "cxSegmentationWidget.h"

#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QStringList>
#include <QVBoxLayout>

#include "sscDataManager.h"
#include "cxDataInterface.h"
#include  "cxVolumePropertiesWidget.h"
#include "cxSegmentation.h"
#include "cxStateMachineManager.h"
#include "cxPatientData.h"

namespace cx
{


SegmentationWidget::SegmentationWidget(QWidget* parent) :
    QWidget(parent)
{
  this->setObjectName("SegmentationWidget");
  this->setWindowTitle("Segmentation");

  //layout
  QVBoxLayout* toptopLayout = new QVBoxLayout(this);
  //toptopLayout->setMargin(0);

//  mImageNameLabel = new QLabel(this);
//  toptopLayout->addWidget(mImageNameLabel);
  toptopLayout->addWidget(new ActiveVolumeWidget(this));

  QPushButton* segmentButton = new QPushButton("Segment", this);
  toptopLayout->addWidget(segmentButton);
  connect(segmentButton, SIGNAL(clicked()), this, SLOT(segmentSlot()));

  QPushButton* contourButton = new QPushButton("Contour", this);
  toptopLayout->addWidget(contourButton);
  connect(contourButton, SIGNAL(clicked()), this, SLOT(contourSlot()));

  //
//  QVBoxLayout* winlvlLayout = new QVBoxLayout;
//  toptopLayout->addLayout(winlvlLayout);
//
//  QGroupBox* group2D = new QGroupBox(this);
//  group2D->setTitle("2D properties");
//  toptopLayout->addWidget(group2D);
//
//  QGridLayout* gridLayout = new QGridLayout(group2D);
//  //toptopLayout->addLayout(gridLayout);
//
//  mLevelWidget = new ssc::SliderGroupWidget(this, ssc::DoubleDataAdapterPtr(new DoubleDataAdapter2DLevel), gridLayout, 0);
//  mWindowWidget = new ssc::SliderGroupWidget(this, ssc::DoubleDataAdapterPtr(new DoubleDataAdapter2DWindow), gridLayout, 1);

  toptopLayout->addStretch();

  //connect(ssc::dataManager(), SIGNAL(activeImageChanged(const std::string&)), this, SLOT(updateSlot()));
  //updateSlot();
}

SegmentationWidget::~SegmentationWidget()
{
}

//void SegmentationWidget::updateSlot()
//{
//  ssc::ImagePtr image = ssc::dataManager()->getActiveImage();
//  if (image)
//  {
//    mImageNameLabel->setText(qstring_cast(image->getName()));
//  }
//}

void SegmentationWidget::showEvent(QShowEvent* event)
{
  QWidget::showEvent(event);
}

void SegmentationWidget::hideEvent(QCloseEvent* event)
{
  QWidget::closeEvent(event);
}

void SegmentationWidget::segmentSlot()
{
  int threshold = 100;
  bool useSmothing = true;
  double smoothSigma = 0.5;

  QString outputBasePath = stateManager()->getPatientData()->getActivePatientFolder();

  Segmentation().segment(ssc::dataManager()->getActiveImage(), outputBasePath, threshold, useSmothing, smoothSigma);
}

void SegmentationWidget::contourSlot()
{
  int threshold = 100;

  QString outputBasePath = stateManager()->getPatientData()->getActivePatientFolder();

  Segmentation().contour(ssc::dataManager()->getActiveImage(), outputBasePath, threshold);
}


}//end namespace cx
