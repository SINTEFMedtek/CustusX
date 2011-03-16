#include "cxTransferFunctionWidget.h"

#include <QVBoxLayout>
#include <QComboBox>
#include <QStringList>
#include <QInputDialog>
#include <QPushButton>
#include "cxTransferFunctionAlphaWidget.h"
#include "cxTransferFunctionColorWidget.h"
#include "sscDataManager.h"
#include "sscImageTF3D.h"
#include "sscImageLUT2D.h"

//#include "sscAbstractInterface.h"
#include "cxShadingParamsInterfaces.h"

namespace cx
{

TransferFunction3DWidget::TransferFunction3DWidget(QWidget* parent) :
  QWidget(parent),
  mLayout(new QVBoxLayout(this))
{
  this->setObjectName("TransferFunction3DWidget");
  this->setWindowTitle("3D");
  this->init();
}

TransferFunction3DWidget::~TransferFunction3DWidget()
{}

void TransferFunction3DWidget::activeImageChangedSlot()
{
//  return;

  ssc::ImagePtr image = ssc::dataManager()->getActiveImage();
  ssc::ImageTFDataPtr tf;
  if (image)
    tf = image->getTransferFunctions3D()->getData();
  else
    image.reset();

  mTransferFunctionAlphaWidget->setData(image, tf);
  mTransferFunctionColorWidget->setData(image, tf);
}

void TransferFunction3DWidget::init()
{
  mTransferFunctionAlphaWidget = new TransferFunctionAlphaWidget(this);
  mTransferFunctionColorWidget = new TransferFunctionColorWidget(this);

  connect(ssc::dataManager(), SIGNAL(activeImageChanged(QString)), this, SLOT(activeImageChangedSlot()));
  connect(ssc::dataManager(), SIGNAL(activeImageTransferFunctionsChanged()), this, SLOT(activeImageChangedSlot()));

  mTransferFunctionAlphaWidget->setSizePolicy(QSizePolicy::MinimumExpanding,
                                              QSizePolicy::MinimumExpanding);
  mTransferFunctionColorWidget->setSizePolicy(QSizePolicy::Expanding,
                                              QSizePolicy::Fixed);

  mLayout->addWidget(mTransferFunctionAlphaWidget);
  mLayout->addWidget(mTransferFunctionColorWidget);

  this->setLayout(mLayout);
}

//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------

TransferFunction2DWidget::TransferFunction2DWidget(QWidget* parent) :
  QWidget(parent),
  mLayout(new QVBoxLayout(this))//,
//  mInitialized(false)
{
  this->setObjectName("TransferFunction2DWidget");
  this->setWindowTitle("2D");
  this->init();
}

TransferFunction2DWidget::~TransferFunction2DWidget()
{}

void TransferFunction2DWidget::activeImageChangedSlot()
{
//  return;
  ssc::ImagePtr image = ssc::dataManager()->getActiveImage();
  ssc::ImageTFDataPtr tf;
  if (image)
    tf = image->getLookupTable2D()->getData();
  else
    image.reset();

  mTransferFunctionAlphaWidget->setData(image, tf);
  mTransferFunctionColorWidget->setData(image, tf);
}

void TransferFunction2DWidget::init()
{
  mTransferFunctionAlphaWidget = new TransferFunctionAlphaWidget(this);
  mTransferFunctionColorWidget = new TransferFunctionColorWidget(this);

  connect(ssc::dataManager(), SIGNAL(activeImageChanged(QString)), this, SLOT(activeImageChangedSlot()));
  connect(ssc::dataManager(), SIGNAL(activeImageTransferFunctionsChanged()), this, SLOT(activeImageChangedSlot()));

  mTransferFunctionAlphaWidget->setSizePolicy(QSizePolicy::MinimumExpanding,
                                              QSizePolicy::MinimumExpanding);
  mTransferFunctionColorWidget->setSizePolicy(QSizePolicy::Expanding,
                                              QSizePolicy::Fixed);

  mLayout->addWidget(mTransferFunctionAlphaWidget);
  mLayout->addWidget(mTransferFunctionColorWidget);

  this->setLayout(mLayout);
}

//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------

TransferFunctionWidget::TransferFunctionWidget(QWidget* parent) :
  QWidget(parent),
  mLayout(new QVBoxLayout(this))
{
  this->setObjectName("TransferFunctionWidget");
  this->setWindowTitle("Transfer Function");

  this->init();
}

TransferFunctionWidget::~TransferFunctionWidget()
{}

//void TransferFunctionWidget::activeImageChangedSlot()
//{
//  ssc::ImagePtr image = ssc::dataManager()->getActiveImage();
//  ssc::ImageTFDataPtr tf;
//  if (image)
//    tf = image->getTransferFunctions3D()->getData();
//  else
//    image.reset();
//
//  mTransferFunctionAlphaWidget->setData(image, tf);
//  mTransferFunctionColorWidget->setData(image, tf);
//}

void TransferFunctionWidget::init()
{
  mTF2DWidget = new TransferFunction2DWidget(this);
  mTF3DWidget = new TransferFunction3DWidget(this);
//
//	mTransferFunctionAlphaWidget = new TransferFunctionAlphaWidget(this);
//	mTransferFunctionColorWidget = new TransferFunctionColorWidget(this);
//
//  connect(ssc::dataManager(), SIGNAL(activeImageChanged(QString)), this, SLOT(activeImageChangedSlot()));
//  connect(ssc::dataManager(), SIGNAL(activeImageTransferFunctionsChanged()), this, SLOT(activeImageChangedSlot()));
//
//  mTransferFunctionAlphaWidget->setSizePolicy(QSizePolicy::MinimumExpanding,
//                                              QSizePolicy::MinimumExpanding);
//  mTransferFunctionColorWidget->setSizePolicy(QSizePolicy::Expanding,
//                                              QSizePolicy::Fixed);
  
  QPushButton* resetButton = new QPushButton("Reset", this);
  connect(resetButton, SIGNAL(clicked()), this, SLOT(resetSlot()));

  QPushButton* saveButton = new QPushButton("Save", this);
  connect(saveButton, SIGNAL(clicked()), this, SLOT(saveSlot()));
  
  mPresetsComboBox = new QComboBox(this);
  mPresetsComboBox->addItems(mPresets.getPresetList());
  connect(mPresetsComboBox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(presetsBoxChangedSlot(const QString&)));

  mLayout->addWidget(mTF2DWidget);
  mLayout->addWidget(mTF3DWidget);
  mLayout->addWidget(mPresetsComboBox);
  //mLayout->addWidget(mInfoWidget);
  QHBoxLayout* buttonLayout = new QHBoxLayout;
  mLayout->addLayout(buttonLayout);

  buttonLayout->addWidget(resetButton);
  buttonLayout->addWidget(saveButton);

  this->setLayout(mLayout);

//  mInitialized = true;
}
  
void TransferFunctionWidget::presetsBoxChangedSlot(const QString& presetName)
{
  ssc::ImagePtr activeImage = ssc::dataManager()->getActiveImage();

  if(!activeImage)
    return;

  mPresets.load(presetName, activeImage);
}

void TransferFunctionWidget::resetSlot()
{
  ssc::ImagePtr activeImage = ssc::dataManager()->getActiveImage();
  activeImage->resetTransferFunctions();
}

void TransferFunctionWidget::saveSlot()
{
    bool ok;
    QString text = QInputDialog::getText(this, "Save Preset",
                                         "Custom Preset Name", QLineEdit::Normal,
                                         "custom preset", &ok);
    if (!ok || text.isEmpty())
    	return;

  ssc::ImagePtr activeImage = ssc::dataManager()->getActiveImage();
  mPresets.save(text, activeImage);

  mPresetsComboBox->clear();
  mPresetsComboBox->addItems(mPresets.getPresetList());

}

}//namespace cx
