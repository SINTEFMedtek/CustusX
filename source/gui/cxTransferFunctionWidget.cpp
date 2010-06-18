#include "cxTransferFunctionWidget.h"

#include <QVBoxLayout>
#include <QComboBox>
#include <QStringList>
#include "cxTransferFunctionAlphaWidget.h"
#include "cxTransferFunctionColorWidget.h"
#include "cxDataManager.h"
#include "sscImageTF3D.h"

//#include "sscAbstractInterface.h"
#include "cxShadingParamsInterfaces.h"

namespace cx
{

TransferFunctionWidget::TransferFunctionWidget(QWidget* parent) :
  QWidget(parent),
  mLayout(new QVBoxLayout(this)),
  mInitialized(false)
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
  connect(dataManager(), SIGNAL(activeImageTransferFunctionsChanged()),
          mTransferFunctionAlphaWidget, SLOT(activeImageTransferFunctionsChangedSlot()));
  connect(dataManager(), SIGNAL(activeImageTransferFunctionsChanged()),
          mTransferFunctionColorWidget, SLOT(activeImageTransferFunctionsChangedSlot()));
  
  mTransferFunctionAlphaWidget->setSizePolicy(QSizePolicy::MinimumExpanding, 
                                              QSizePolicy::MinimumExpanding);
  mTransferFunctionColorWidget->setSizePolicy(QSizePolicy::Expanding, 
                                              QSizePolicy::Fixed);
  
  mShadingCheckBox = new QCheckBox("Shading", this);
  mPresetsComboBox = new QComboBox(this);
  
  //Populate presets comboBox
  /*mPresets = new QStringList();
  mPresets->append("Transfer function preset...");
  mPresets->append("Fire - CT");
  mPresets->append("Blue - CT");*/

  mPresetsComboBox->addItems(mPresets.getPresetList());
  //this->initTransferFunctionPresets();
  
  connect(mShadingCheckBox, SIGNAL(toggled(bool)), 
          this, SLOT(shadingToggledSlot(bool)));
  connect(mPresetsComboBox, SIGNAL(currentIndexChanged(const QString&)),
          this, SLOT(presetsBoxChangedSlot(const QString&)));

  mLayout->addWidget(mTransferFunctionAlphaWidget);
  mLayout->addWidget(mTransferFunctionColorWidget);
  mLayout->addWidget(mPresetsComboBox);
  //mLayout->addWidget(mInfoWidget);
  
  
  QGridLayout* shadingLayput = new QGridLayout();
  
  ssc::SliderGroupWidget* shadingAmbientWidget = new ssc::SliderGroupWidget(this, ssc::DoubleDataInterfacePtr(new DoubleDataInterfaceShadingAmbient()), shadingLayput, 0);
  ssc::SliderGroupWidget* shadingDiffuseWidget = new ssc::SliderGroupWidget(this, ssc::DoubleDataInterfacePtr(new DoubleDataInterfaceShadingDiffuse()), shadingLayput, 1);
  ssc::SliderGroupWidget* shadingSpecularWidget = new ssc::SliderGroupWidget(this, ssc::DoubleDataInterfacePtr(new DoubleDataInterfaceShadingSpecular()), shadingLayput, 2);
  ssc::SliderGroupWidget* shadingSpecularPowerWidget = new ssc::SliderGroupWidget(this, ssc::DoubleDataInterfacePtr(new DoubleDataInterfaceShadingSpecularPower()), shadingLayput, 3);
  
  
  shadingAmbientWidget->setEnabled(false);
  shadingDiffuseWidget->setEnabled(false);
  shadingSpecularWidget->setEnabled(false);
  shadingSpecularPowerWidget->setEnabled(false);
  
  //QLabel* shadingLabel = new QLabel("Shading", this);
  mLayout->addWidget(mShadingCheckBox);
  mLayout->addLayout(shadingLayput);
  
  //mLayout->addWidget(shadingLabel);
  //mLayout->addWidget(shadingAmbientWidget);
  //mLayout->addWidget(shadingDiffuseWidget);
  //mLayout->addWidget(shadingSpecularWidget);
  //mLayout->addWidget(shadingSpecularPowerWidget);
  
  this->setLayout(mLayout);

  mInitialized = true;
}

void TransferFunctionWidget::shadingToggledSlot(bool val)
{
  ssc::ImagePtr image = dataManager()->getActiveImage();
  if (image)
  {
    image->setShadingOn(val);
  }
}

void TransferFunctionWidget::activeImageChangedSlot()
{
  if (!mInitialized)
    init();

  ssc::ImagePtr activeImage = dataManager()->getActiveImage();
  if(mCurrentImage == activeImage)
    return;

  mCurrentImage = activeImage;

  if (activeImage)
  {
    mShadingCheckBox->setChecked(activeImage->getShadingOn());
  }
}

/*MOVED TO PresetTransferFunctions3D
 void TransferFunctionWidget::initTransferFunctionPresets()
{
  // Use XML structure
  QDomDocument doc;
  mTransferfunctionPresetCTFire = doc.createElement("transferfunctions");
  QDomElement alphaNode = doc.createElement("alpha");
  QStringList pointStringList;
  // Add alpha points
  pointStringList.append(QString("0=0"));
  pointStringList.append(QString("100=100"));
  pointStringList.append(QString("150=100"));
  pointStringList.append(QString("1000=200"));
  alphaNode.appendChild(doc.createTextNode(pointStringList.join(" ")));
  pointStringList.clear();
  
  QDomElement colorNode = doc.createElement("color");  
  // Add color points
  pointStringList.append(QString("0=0/0/0"));
  pointStringList.append(QString("150=255/255/0"));
  pointStringList.append(QString("1000=255/0/0"));
  colorNode.appendChild(doc.createTextNode(pointStringList.join(" ")));
  pointStringList.clear();
  
  mTransferfunctionPresetCTFire.appendChild(alphaNode);
  mTransferfunctionPresetCTFire.appendChild(colorNode);
  
  
  mTransferfunctionPresetCTBlue = doc.createElement("transferfunctions");
  alphaNode = doc.createElement("alpha");
  // Add alpha points
  pointStringList.append(QString("0=0"));
  pointStringList.append(QString("200=50"));
  pointStringList.append(QString("1000=255"));
  alphaNode.appendChild(doc.createTextNode(pointStringList.join(" ")));
  pointStringList.clear();
  
  colorNode = doc.createElement("color");  
  // Add color points
  pointStringList.append(QString("0=0/0/0"));
  pointStringList.append(QString("200=255/255/0"));
  pointStringList.append(QString("1000=0/0/255"));
  colorNode.appendChild(doc.createTextNode(pointStringList.join(" ")));
  pointStringList.clear();
  
  mTransferfunctionPresetCTBlue.appendChild(alphaNode);
  mTransferfunctionPresetCTBlue.appendChild(colorNode);
}*/
  
void TransferFunctionWidget::presetsBoxChangedSlot(const QString& presetName)
{
  if(!mCurrentImage)
    return;
  
  ssc::ImageTF3DPtr transferFunctions = mCurrentImage->getTransferFunctions3D();
  transferFunctions->parseXml(mPresets.getPresetDomElement(presetName));

  mCurrentImage->setShading(mPresets.getShadingPresets(presetName));
  
  mShadingCheckBox->setChecked(mCurrentImage->getShadingOn());
  
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
  transferFunctions->addAlphaPoint(mCurrentImage->getMin(), 0);
  transferFunctions->addAlphaPoint(mCurrentImage->getMax(), 0);
  transferFunctions->addColorPoint(mCurrentImage->getMin(), QColor(0,0,0));
  transferFunctions->addColorPoint(mCurrentImage->getMax(), QColor(0,0,0));
}
  
}//namespace cx
