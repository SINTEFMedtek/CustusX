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
#include "cxDataSelectWidget.h"
#include "cxSelectDataStringDataAdapter.h"

namespace cx
{

//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------

TransferFunction2DColorWidget::TransferFunction2DColorWidget(QWidget* parent) :
  BaseWidget(parent, "TransferFunction2DColorWidget", "2D Color")
{
  QVBoxLayout* layout = new QVBoxLayout(this);

  mTransferFunctionColorWidget = new TransferFunctionColorWidget(this);
  mTransferFunctionAlphaWidget = new TransferFunctionAlphaWidget(this);
  mTransferFunctionAlphaWidget->setReadOnly(true);

  mDataWindow.reset(new DoubleDataAdapterImageTFDataWindow);
  mDataLevel.reset(new DoubleDataAdapterImageTFDataLevel);

  mActiveImageProxy = ActiveImageProxy::New();
  connect(mActiveImageProxy.get(), SIGNAL(activeImageChanged(QString)), this, SLOT(activeImageChangedSlot()));
  connect(mActiveImageProxy.get(), SIGNAL(transferFunctionsChanged()), this, SLOT(activeImageChangedSlot()));

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

  this->setLayout(layout);
}

QString TransferFunction2DColorWidget::defaultWhatsThis() const
{
  return "<html>"
    "<h3>2D color transfer function.</h3>"
    "<p>Lets you set a 2D color transfer function.</p>"
    "<p><i></i></p>"
    "</html>";
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
  BaseWidget(parent, "TransferFunction2DOpacityWidget", "2D Opacity")
{
  QVBoxLayout* layout = new QVBoxLayout(this);

  mTransferFunctionAlphaWidget = new TransferFunctionAlphaWidget(this);
  mTransferFunctionAlphaWidget->setReadOnly(true);

  mDataAlpha.reset(new DoubleDataAdapterImageTFDataAlpha);
  mDataLLR.reset(new DoubleDataAdapterImageTFDataLLR);

  mActiveImageProxy = ActiveImageProxy::New();
  connect(mActiveImageProxy.get(), SIGNAL(activeImageChanged(QString)), this, SLOT(activeImageChangedSlot()));
  connect(mActiveImageProxy.get(), SIGNAL(transferFunctionsChanged()), this, SLOT(activeImageChangedSlot()));

  mTransferFunctionAlphaWidget->setSizePolicy(QSizePolicy::MinimumExpanding,
                                              QSizePolicy::MinimumExpanding);

  layout->addWidget(mTransferFunctionAlphaWidget);

  QGridLayout* gridLayout = new QGridLayout;
  layout->addLayout(gridLayout);
  new ssc::SliderGroupWidget(this, mDataAlpha,  gridLayout, 2);
  new ssc::SliderGroupWidget(this, mDataLLR,    gridLayout, 3);

  this->setLayout(layout);
}

QString TransferFunction2DOpacityWidget::defaultWhatsThis() const
{
  return "<html>"
    "<h3>2D opacity transfer function.</h3>"
    "<p>Lets you set a 2D opacity transfer function.</p>"
    "<p><i></i></p>"
    "</html>";
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
    BaseWidget(parent, "ImagePropertiesWidget", "Slice Properties")
{
  QVBoxLayout* toptopLayout = new QVBoxLayout(this);

////  toptopLayout->addWidget(new ssc::LabeledComboBoxWidget(this, ActiveImageStringDataAdapter::New()));
  toptopLayout->addWidget(new DataSelectWidget(this, ActiveImageStringDataAdapter::New()));

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
  colorWidgetLayout->addWidget(new TransferFunctionPresetWidget(this, false), 0);

  QTabWidget* tabWidget = new QTabWidget(this);
  toptopLayout->addWidget(tabWidget);
  tabWidget->addTab(new VolumeInfoWidget(this), "Info");
  tabWidget->addTab(colorWidget, QString("Color"));
  tabWidget->addTab(overlayWidget, QString("Overlay"));
}

ImagePropertiesWidget::~ImagePropertiesWidget()
{}

QString ImagePropertiesWidget::defaultWhatsThis() const
{
  return "<html>"
    "<h3>Image slice properties.</h3>"
    "<p>Lets you set properties on a 2d image slice.</p>"
    "<p><i></i></p>"
    "</html>";
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
