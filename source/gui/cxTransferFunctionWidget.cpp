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

//#include "sscAbstractInterface.h"
#include "cxShadingParamsInterfaces.h"

namespace cx
{

TransferFunctionWidget::TransferFunctionWidget(QWidget* parent) :
  QWidget(parent),
  mLayout(new QVBoxLayout(this)),
  mInitialized(false)
{
  this->setObjectName("TransferFunctionWidget");
  this->setWindowTitle("Transfer Function");

  this->init();
}

TransferFunctionWidget::~TransferFunctionWidget()
{}

void TransferFunctionWidget::init()
{
	mTransferFunctionAlphaWidget = new TransferFunctionAlphaWidget(this);
	mTransferFunctionColorWidget = new TransferFunctionColorWidget(this);

  connect(ssc::dataManager(), SIGNAL(activeImageChanged(QString)),
          mTransferFunctionAlphaWidget, SLOT(activeImageChangedSlot()));
  connect(ssc::dataManager(), SIGNAL(activeImageChanged(QString)),
          mTransferFunctionColorWidget, SLOT(activeImageChangedSlot()));
  connect(ssc::dataManager(), SIGNAL(activeImageTransferFunctionsChanged()),
          mTransferFunctionAlphaWidget, SLOT(activeImageTransferFunctionsChangedSlot()));
  connect(ssc::dataManager(), SIGNAL(activeImageTransferFunctionsChanged()),
          mTransferFunctionColorWidget, SLOT(activeImageTransferFunctionsChangedSlot()));
  
  mTransferFunctionAlphaWidget->setSizePolicy(QSizePolicy::MinimumExpanding, 
                                              QSizePolicy::MinimumExpanding);
  mTransferFunctionColorWidget->setSizePolicy(QSizePolicy::Expanding, 
                                              QSizePolicy::Fixed);
  
  mPresetsComboBox = new QComboBox(this);

  QPushButton* resetButton = new QPushButton("Reset", this);
  connect(resetButton, SIGNAL(clicked()), this, SLOT(resetSlot()));

  QPushButton* saveButton = new QPushButton("Save", this);
  connect(saveButton, SIGNAL(clicked()), this, SLOT(saveSlot()));
  
  mPresetsComboBox->addItems(mPresets.getPresetList());
  
  connect(mPresetsComboBox, SIGNAL(currentIndexChanged(const QString&)),
          this, SLOT(presetsBoxChangedSlot(const QString&)));

  mLayout->addWidget(mTransferFunctionAlphaWidget);
  mLayout->addWidget(mTransferFunctionColorWidget);
  mLayout->addWidget(mPresetsComboBox);
  //mLayout->addWidget(mInfoWidget);
  QHBoxLayout* buttonLayout = new QHBoxLayout;
  mLayout->addLayout(buttonLayout);

  buttonLayout->addWidget(resetButton);
  buttonLayout->addWidget(saveButton);

  this->setLayout(mLayout);

  mInitialized = true;
}
  
void TransferFunctionWidget::presetsBoxChangedSlot(const QString& presetName)
{
  ssc::ImagePtr activeImage = ssc::dataManager()->getActiveImage();

  if(!activeImage)
    return;

  mPresets.load(presetName, activeImage);

//  ssc::ImageTF3DPtr transferFunctions = activeImage->getTransferFunctions3D();
//  transferFunctions->parseXml(mPresets.getPresetDomElement(presetName));
//
//  activeImage->setShading(mPresets.getShadingPresets(presetName));
//
  //mShadingCheckBox->setChecked(activeImage->getShadingOn());
  
  //transferFunctions->addAlphaPoint(0, 0);
  //transferFunctions->addColorPoint(0, QColor(0,0,0));
  
  /*switch(val)
  {
    case 1:
      //transferFunctions->parseXml(mTransferfunctionPresetCTFire);
      break;
    case 2:
      transferFunctions->parseXml(mTransferfunctionPresetCTBlue);
      break;
    default:
      break;
  }*/

//  //Make sure min and max values for transferfunctions are set
//  transferFunctions->addAlphaPoint(activeImage->getMin(), 0);
//  transferFunctions->addAlphaPoint(activeImage->getMax(), 0);
//  transferFunctions->addColorPoint(activeImage->getMin(), QColor(0,0,0));
//  transferFunctions->addColorPoint(activeImage->getMax(), QColor(0,0,0));
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
