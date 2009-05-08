#include "cxTransferFunctionWidget.h"

#include <QVBoxLayout>
#include "cxTransferFunctionAlphaWidget.h"
#include "cxTransferFunctionColorWidget.h"

namespace cx
{

TransferFunctionWidget::TransferFunctionWidget(QWidget* parent) :
  QWidget(parent),
  mLayout(new QVBoxLayout(this)),
	mInitialized(false)
  //mTransferFunctionAlphaWidget(new TransferFunctionAlphaWidget(this)),
  //mTransferFunctionColorWidget(new TransferFunctionColorWidget(this))
{
}
TransferFunctionWidget::~TransferFunctionWidget()
{}

void TransferFunctionWidget::init()
{
	mTransferFunctionAlphaWidget = new TransferFunctionAlphaWidget(this);
	mTransferFunctionColorWidget = new TransferFunctionColorWidget(this);
  
	connect(this, SIGNAL(currentImageChanged(ssc::ImagePtr)),
					mTransferFunctionAlphaWidget, SLOT(currentImageChangedSlot(ssc::ImagePtr)));
  connect(this, SIGNAL(currentImageChanged(ssc::ImagePtr)),
					mTransferFunctionColorWidget, SLOT(currentImageChangedSlot(ssc::ImagePtr)));
	
  mTransferFunctionAlphaWidget->setSizePolicy(QSizePolicy::MinimumExpanding, 
                                              QSizePolicy::MinimumExpanding);
  mTransferFunctionColorWidget->setSizePolicy(QSizePolicy::Expanding, 
                                              QSizePolicy::Fixed);
  
  mLayout->addWidget(mTransferFunctionAlphaWidget);
  mLayout->addWidget(mTransferFunctionColorWidget);
  //mLayout->addWidget(mInfoWidget);
  this->setLayout(mLayout);
	
	mInitialized = true;
}
	
void TransferFunctionWidget::currentImageChangedSlot(ssc::ImagePtr currentImage)
{
  if(mCurrentImage == currentImage)
    return;

	if (!mInitialized)
		init();
	
  mCurrentImage = currentImage;
  emit currentImageChanged(mCurrentImage);

  //this->updatesomething()
}
}//namespace cx
