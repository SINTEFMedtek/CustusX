#include "cxTransferFunctionWidget.h"

#include <QVBoxLayout>
#include "cxTransferFunctionAlphaWidget.h"
#include "cxTransferFunctionColorWidget.h"
#include "cxDataManager.h"

namespace cx
{

TransferFunctionWidget::TransferFunctionWidget(QWidget* parent) :
  QWidget(parent),
  mLayout(new QVBoxLayout(this)),
	mInitialized(false)
  //mTransferFunctionAlphaWidget(new TransferFunctionAlphaWidget(this)),
  //mTransferFunctionColorWidget(new TransferFunctionColorWidget(this))
{}
TransferFunctionWidget::~TransferFunctionWidget()
{}
void TransferFunctionWidget::init()
{
	mTransferFunctionAlphaWidget = new TransferFunctionAlphaWidget(this);
	mTransferFunctionColorWidget = new TransferFunctionColorWidget(this);

  connect(dataManager(), SIGNAL(activeImageChanged(std::string)),
          mTransferFunctionAlphaWidget, SLOT(activeImageChangedSlot()));
  connect(dataManager(), SIGNAL(activeImageChanged(std::string)),
          mTransferFunctionColorWidget, SLOT(activeImageChangedSlot()));

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
	
void TransferFunctionWidget::activeImageChangedSlot()
{
  ssc::ImagePtr activeImage = dataManager()->getActiveImage();
  if(mCurrentImage == activeImage)
    return;

	if (!mInitialized)
		init();
	
  mCurrentImage = activeImage;

  //emit currentImageChanged(mCurrentImage);

  //this->updatesomething()
}
}//namespace cx
