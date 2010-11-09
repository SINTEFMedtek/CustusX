#include "cxImagePropertiesWidget.h"

#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QStringList>
#include <QVBoxLayout>

#include "cxDataInterface.h"
#include "sscLabeledComboBoxWidget.h"

namespace cx
{
ImagePropertiesWidget::ImagePropertiesWidget(QWidget* parent) :
    QWidget(parent)
{
  this->setObjectName("ImagePropertiesWidget");
  this->setWindowTitle("Slice Properties");

  QVBoxLayout* toptopLayout = new QVBoxLayout(this);

  toptopLayout->addWidget(new ssc::LabeledComboBoxWidget(this, ActiveImageStringDataAdapter::New()));

  QVBoxLayout* winlvlLayout = new QVBoxLayout;
  toptopLayout->addLayout(winlvlLayout);

  QGroupBox* group2D = new QGroupBox(this);
  group2D->setTitle("2D properties");
  toptopLayout->addWidget(group2D);

  QGridLayout* gridLayout = new QGridLayout(group2D);

  mLevelWidget = new ssc::SliderGroupWidget(this, ssc::DoubleDataAdapterPtr(new DoubleDataAdapter2DLevel), gridLayout, 0);
  mWindowWidget = new ssc::SliderGroupWidget(this, ssc::DoubleDataAdapterPtr(new DoubleDataAdapter2DWindow), gridLayout, 1);

  toptopLayout->addStretch();
}

ImagePropertiesWidget::~ImagePropertiesWidget()
{}

void ImagePropertiesWidget::showEvent(QShowEvent* event)
{
  QWidget::showEvent(event);
}

void ImagePropertiesWidget::hideEvent(QCloseEvent* event)
{
  QWidget::closeEvent(event);
}


}//end namespace cx
