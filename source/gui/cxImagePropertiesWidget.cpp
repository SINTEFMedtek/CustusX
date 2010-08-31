#include "cxImagePropertiesWidget.h"

#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QStringList>
#include <QVBoxLayout>
#include "sscImage.h"
#include "sscMessageManager.h"
#include "cxDataManager.h"
#include "sscToolManager.h"
#include "cxViewManager.h"
#include "cxRepManager.h"
#include "cxView2D.h"
#include "cxView3D.h"
#include "sscImageLUT2D.h"
#include "cxDataInterface.h"
#include  "cxVolumePropertiesWidget.h"

namespace cx
{


ImagePropertiesWidget::ImagePropertiesWidget(QWidget* parent) :
    QWidget(parent)
{
  this->setObjectName("ImagePropertiesWidget");
  this->setWindowTitle("Slice Properties");

  //layout
  QVBoxLayout* toptopLayout = new QVBoxLayout(this);
  //toptopLayout->setMargin(0);

//  mImageNameLabel = new QLabel(this);
//  toptopLayout->addWidget(mImageNameLabel);
  toptopLayout->addWidget(new ActiveVolumeWidget(this));

  QVBoxLayout* winlvlLayout = new QVBoxLayout;
  toptopLayout->addLayout(winlvlLayout);

  QGroupBox* group2D = new QGroupBox(this);
  group2D->setTitle("2D properties");
  toptopLayout->addWidget(group2D);

  QGridLayout* gridLayout = new QGridLayout(group2D);
  //toptopLayout->addLayout(gridLayout);

  mLevelWidget = new ssc::SliderGroupWidget(this, ssc::DoubleDataAdapterPtr(new DoubleDataAdapter2DLevel), gridLayout, 0);
  mWindowWidget = new ssc::SliderGroupWidget(this, ssc::DoubleDataAdapterPtr(new DoubleDataAdapter2DWindow), gridLayout, 1);

  toptopLayout->addStretch();

  //connect(ssc::dataManager(), SIGNAL(activeImageChanged(const std::string&)), this, SLOT(updateSlot()));
  //updateSlot();
}

ImagePropertiesWidget::~ImagePropertiesWidget()
{
}

//void ImagePropertiesWidget::updateSlot()
//{
//  ssc::ImagePtr image = ssc::dataManager()->getActiveImage();
//  if (image)
//  {
//    mImageNameLabel->setText(qstring_cast(image->getName()));
//  }
//}

void ImagePropertiesWidget::showEvent(QShowEvent* event)
{
  QWidget::showEvent(event);
}

void ImagePropertiesWidget::hideEvent(QCloseEvent* event)
{
  QWidget::closeEvent(event);
}


}//end namespace cx
