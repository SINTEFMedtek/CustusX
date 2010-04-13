#include "cxImagePropertiesWidget.h"

#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QStringList>
#include <QVBoxLayout>
#include "sscImage.h"
#include "sscTypeConversions.h"
#include "cxDataManager.h"
#include "cxToolManager.h"
#include "cxMessageManager.h"
#include "cxViewManager.h"
#include "cxRepManager.h"
#include "cxView2D.h"
#include "cxView3D.h"
#include "sscImageLUT2D.h"
#include "cxDataInterface.h"

namespace cx
{


ImagePropertiesWidget::ImagePropertiesWidget(QWidget* parent) :
    QWidget(parent)
{
  this->setObjectName("ImagePropertiesWidget");
  this->setWindowTitle("ImageProperties");

  //layout
  QVBoxLayout* toptopLayout = new QVBoxLayout(this);
  //toptopLayout->setMargin(0);

  mImageNameLabel = new QLabel(this);
  toptopLayout->addWidget(mImageNameLabel);

  QVBoxLayout* winlvlLayout = new QVBoxLayout;
  toptopLayout->addLayout(winlvlLayout);

  QGridLayout* gridLayout = new QGridLayout;
  toptopLayout->addLayout(gridLayout);

  mLevelWidget = new SliderGroupWidget(this, ssc::DoubleDataInterfacePtr(new DoubleDataInterface2DLevel), gridLayout, 0);
  mWindowWidget = new SliderGroupWidget(this, ssc::DoubleDataInterfacePtr(new DoubleDataInterface2DWindow), gridLayout, 1);

  connect(dataManager(), SIGNAL(activeImageChanged(const std::string&)), this, SLOT(updateSlot()));
  updateSlot();
}

ImagePropertiesWidget::~ImagePropertiesWidget()
{
}

void ImagePropertiesWidget::updateSlot()
{
  ssc::ImagePtr image = dataManager()->getActiveImage();
  if (image)
  {
    mImageNameLabel->setText(qstring_cast(image->getName()));
  }
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
