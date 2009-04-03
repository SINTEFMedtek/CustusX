#include "cxTransferFunctionWidget.h"

#include <QVBoxLayout>
//#include "cxTransferFunctionAlphaWidget.h"
//#include "cxTransferFunctionColorWidget.h"

namespace cx
{

TransferFunctionWidget::TransferFunctionWidget(QWidget* parent) :
  QWidget(parent),
  mLayout(new QVBoxLayout(this))
  //mTransferFunctionAlphaWidget(new TransferFunctionAlphaWidget(this))
{
/*  connect(this, SIGNAL(currentImageChanged(ssc::ImagePtr)),
      mTransferFunctionAlphaWidget, SLOT(currentImageChangedSlot(ssc::ImagePtr)));
  connect(this, SIGNAL(currentImageChanged(ssc::ImagePtr)),
      mTransferFunctionColorWidget, SLOT(currentImageChangedSlot(ssc::ImagePtr)));

  mLayout->addWidget(mTransferFunctionAlphaWidget);
  mLayout->addWidget(mTransferFunctionColorWidget);*/
  //mLayout->addWidget(mInfoWidget);
  this->setLayout(mLayout);
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
