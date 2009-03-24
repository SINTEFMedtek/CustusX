#include "cxTransferFunctionWidget.h"

#include <QVBoxLayout>
#include "cxTransferFunctionAlphaWidget.h"

namespace cx
{

TransferFunctionWidget::TransferFunctionWidget(QWidget* parent) :
  QWidget(parent),
  mLayout(new QVBoxLayout(this)),
  mTransferFunctionAlphaWidget(new TransferFunctionAlphaWidget(this))
{
  connect(this, SIGNAL(currentImageChanged(ssc::ImagePtr)),
      mTransferFunctionAlphaWidget, SLOT(currentImageChangedSlot(ssc::ImagePtr)));
/*  connect(this, SIGNAL(currentImageChanged(ssc::ImagePtr)),
      mTransferFunctionRgbWidget, SLOT(currentImageChangedSlot(ssc::ImagePtr)));*/

  mLayout->addWidget(mTransferFunctionAlphaWidget);
  //§mLayout->addWidget(mTransferFunctionRgbWidget);
  //mLayout->addWidget(mInfoWidget);
}
TransferFunctionWidget::~TransferFunctionWidget()
{}
void TransferFunctionWidget::currentImageChangedSlot(ssc::ImagePtr currentImage)
{
  if(mCurrentImage == currentImage)
    return;

  mCurrentImage = currentImage;
  emit currentImageChanged(mCurrentImage);

  //this->updatesomething()
}
}//namespace cx
