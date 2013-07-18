#include "cxTransferFunction2DOpacityWidget.h"
#include "sscDoubleWidgets.h"
#include "sscDataManager.h"
#include "sscImageLUT2D.h"
namespace cx
{

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


}//end namespace cx
