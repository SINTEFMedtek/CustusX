#include "cxImagePropertiesWidget.h"

#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QStringList>
#include <QVBoxLayout>

#include "sscDataManager.h"
#include "sscImage.h"
#include "sscImageLUT2D.h"
#include "cxDataInterface.h"
#include "sscLabeledComboBoxWidget.h"
#include "cxTransferFunctionWidget.h"
#include "cxVolumePropertiesWidget.h"
#include "cxDataViewSelectionWidget.h"
#include "cxTransferFunctionAlphaWidget.h"
#include "cxTransferFunctionColorWidget.h"
#include "cxViewWrapper2D.h"

namespace cx
{

//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------

TransferFunction2DColorWidget::TransferFunction2DColorWidget(QWidget* parent) :
  QWidget(parent)
{
  QVBoxLayout* layout = new QVBoxLayout(this);

  this->setObjectName("TransferFunction2DColorWidget");
  this->setWindowTitle("2D Color");

  mTransferFunctionColorWidget = new TransferFunctionColorWidget(this);
  mTransferFunctionAlphaWidget = new TransferFunctionAlphaWidget(this);
  mTransferFunctionAlphaWidget->setReadOnly(true);

  mDataWindow.reset(new DoubleDataAdapterImageTFDataWindow);
  mDataLevel.reset(new DoubleDataAdapterImageTFDataLevel);

  connect(ssc::dataManager(), SIGNAL(activeImageChanged(QString)), this, SLOT(activeImageChangedSlot()));
  connect(ssc::dataManager(), SIGNAL(activeImageTransferFunctionsChanged()), this, SLOT(activeImageChangedSlot()));

  mTransferFunctionColorWidget->setSizePolicy(QSizePolicy::Expanding,
                                              QSizePolicy::Fixed);
  mTransferFunctionAlphaWidget->setSizePolicy(QSizePolicy::MinimumExpanding,
                                              QSizePolicy::MinimumExpanding);

  layout->addWidget(mTransferFunctionAlphaWidget);

  layout->addWidget(mTransferFunctionColorWidget);

  QGridLayout* gridLayout = new QGridLayout;
  layout->addLayout(gridLayout);
  new ssc::SliderGroupWidget(this, mDataWindow, gridLayout, 0);
  new ssc::SliderGroupWidget(this, mDataLevel,  gridLayout, 1);
//  mLayout->addWidget(new TransferFunctionPresetWidget(this));
//  mLayout->addStretch();

  this->setLayout(layout);
}

void TransferFunction2DColorWidget::activeImageChangedSlot()
{
  ssc::ImagePtr image = ssc::dataManager()->getActiveImage();
  ssc::ImageTFDataPtr tf;
  if (image)
    tf = image->getLookupTable2D();
  else
    image.reset();

  mTransferFunctionColorWidget->setData(image, tf);
  mTransferFunctionAlphaWidget->setData(image, tf);

  mDataWindow->setImageTFData(tf);
  mDataLevel->setImageTFData(tf);
}

//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------

TransferFunction2DOpacityWidget::TransferFunction2DOpacityWidget(QWidget* parent) :
  QWidget(parent)
{
  QVBoxLayout* layout = new QVBoxLayout(this);

  this->setObjectName("TransferFunction2DOpacityWidget");
  this->setWindowTitle("2D Opacity");

  mTransferFunctionAlphaWidget = new TransferFunctionAlphaWidget(this);
  mTransferFunctionAlphaWidget->setReadOnly(true);

  mDataAlpha.reset(new DoubleDataAdapterImageTFDataAlpha);
  mDataLLR.reset(new DoubleDataAdapterImageTFDataLLR);

  connect(ssc::dataManager(), SIGNAL(activeImageChanged(QString)), this, SLOT(activeImageChangedSlot()));
  connect(ssc::dataManager(), SIGNAL(activeImageTransferFunctionsChanged()), this, SLOT(activeImageChangedSlot()));

  mTransferFunctionAlphaWidget->setSizePolicy(QSizePolicy::MinimumExpanding,
                                              QSizePolicy::MinimumExpanding);

  layout->addWidget(mTransferFunctionAlphaWidget);

  QGridLayout* gridLayout = new QGridLayout;
  layout->addLayout(gridLayout);
  new ssc::SliderGroupWidget(this, mDataAlpha,  gridLayout, 2);
  new ssc::SliderGroupWidget(this, mDataLLR,    gridLayout, 3);

  this->setLayout(layout);
}

void TransferFunction2DOpacityWidget::activeImageChangedSlot()
{
  ssc::ImagePtr image = ssc::dataManager()->getActiveImage();
  ssc::ImageTFDataPtr tf;
  if (image)
    tf = image->getLookupTable2D();
  else
    image.reset();

  mTransferFunctionAlphaWidget->setData(image, tf);

  mDataAlpha->setImageTFData(tf);
  mDataLLR->setImageTFData(tf);
}

//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------



ImagePropertiesWidget::ImagePropertiesWidget(QWidget* parent) :
    QWidget(parent)
{
  this->setObjectName("ImagePropertiesWidget");
  this->setWindowTitle("Slice Properties");

  QVBoxLayout* toptopLayout = new QVBoxLayout(this);

  toptopLayout->addWidget(new ssc::LabeledComboBoxWidget(this, ActiveImageStringDataAdapter::New()));

  // widget with opacity and volume ordering
  QWidget* overlayWidget = new QWidget(this);
  QVBoxLayout* overlayWidgetLayout = new QVBoxLayout(overlayWidget);
  overlayWidgetLayout->setMargin(0);
  overlayWidgetLayout->addWidget(new TransferFunction2DOpacityWidget(this), 0);
  overlayWidgetLayout->addWidget(new DataViewSelectionWidget(this), 1);
  overlayWidget->setEnabled(ViewWrapper2D::overlayIsEnabled());

  QWidget* colorWidget = new QWidget(this);
  QVBoxLayout* colorWidgetLayout = new QVBoxLayout(colorWidget);
  colorWidgetLayout->setMargin(0);
  colorWidgetLayout->addWidget(new TransferFunction2DColorWidget(this), 0);
  colorWidgetLayout->addStretch(1);
  colorWidgetLayout->addWidget(new TransferFunctionPresetWidget(this), 0);

  QTabWidget* tabWidget = new QTabWidget(this);
  toptopLayout->addWidget(tabWidget);
  tabWidget->addTab(new VolumeInfoWidget(this), "Info");
  tabWidget->addTab(colorWidget, QString("Color"));
  tabWidget->addTab(overlayWidget, QString("Overlay"));
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
