#include "cxTransferFunctionWidget.h"

#include <QVBoxLayout>
#include <QComboBox>
#include <QStringList>
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
  
  //Populate presets comboBox
  /*mPresets = new QStringList();
  mPresets->append("Transfer function preset...");
  mPresets->append("Fire - CT");
  mPresets->append("Blue - CT");*/

  mPresetsComboBox->addItems(mPresets.getPresetList());
  //this->initTransferFunctionPresets();
  
  connect(mPresetsComboBox, SIGNAL(currentIndexChanged(const QString&)),
          this, SLOT(presetsBoxChangedSlot(const QString&)));

  mLayout->addWidget(mTransferFunctionAlphaWidget);
  mLayout->addWidget(mTransferFunctionColorWidget);
  mLayout->addWidget(mPresetsComboBox);
  //mLayout->addWidget(mInfoWidget);

  this->setLayout(mLayout);

  mInitialized = true;
}
  
void TransferFunctionWidget::presetsBoxChangedSlot(const QString& presetName)
{
  ssc::ImagePtr activeImage = ssc::dataManager()->getActiveImage();

  if(!activeImage)
    return;
  
  ssc::ImageTF3DPtr transferFunctions = activeImage->getTransferFunctions3D();
  transferFunctions->parseXml(mPresets.getPresetDomElement(presetName));

  activeImage->setShading(mPresets.getShadingPresets(presetName));
  
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

  //Make sure min and max values for transferfunctions are set
  transferFunctions->addAlphaPoint(activeImage->getMin(), 0);
  transferFunctions->addAlphaPoint(activeImage->getMax(), 0);
  transferFunctions->addColorPoint(activeImage->getMin(), QColor(0,0,0));
  transferFunctions->addColorPoint(activeImage->getMax(), QColor(0,0,0));
}
  
}//namespace cx
