#include "cxTransferFunction2DColorWidget.h"
#include "cxDoubleWidgets.h"
#include "cxDataManager.h"
#include "cxImageLUT2D.h"

namespace cx
{

TransferFunction2DColorWidget::TransferFunction2DColorWidget(QWidget* parent) :
  BaseWidget(parent, "TransferFunction2DColorWidget", "2D Color")
{
  QVBoxLayout* layout = new QVBoxLayout(this);

  mTransferFunctionColorWidget = new TransferFunctionColorWidget(this);
  mTransferFunctionAlphaWidget = new TransferFunctionAlphaWidget(this);
  mTransferFunctionAlphaWidget->setReadOnly(true);

  mDataWindow.reset(new DoubleDataAdapterImageTFDataWindow);
  mDataLevel.reset(new DoubleDataAdapterImageTFDataLevel);

  mActiveImageProxy = ActiveImageProxy::New(dataService());
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
  new SliderGroupWidget(this, mDataWindow, gridLayout, 0);
  new SliderGroupWidget(this, mDataLevel,  gridLayout, 1);

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
  ImagePtr image = dataManager()->getActiveImage();
  ImageTFDataPtr tf;
  if (image)
    tf = image->getLookupTable2D();
  else
    image.reset();

  mTransferFunctionColorWidget->setData(image, tf);
  mTransferFunctionAlphaWidget->setData(image, tf);

  mDataWindow->setImageTFData(tf, image);
  mDataLevel->setImageTFData(tf, image);
}


}//end namespace cx
