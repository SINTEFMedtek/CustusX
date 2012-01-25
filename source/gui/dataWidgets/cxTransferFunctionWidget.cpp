#include "cxTransferFunctionWidget.h"

#include <QVBoxLayout>
#include <QComboBox>
#include <QStringList>
#include <QInputDialog>
#include <QPushButton>
#include <QMessageBox>
#include "cxTransferFunctionAlphaWidget.h"
#include "cxTransferFunctionColorWidget.h"
#include "sscDataManager.h"
#include "sscImageTF3D.h"
#include "sscImageLUT2D.h"
#include "sscMessageManager.h"
#include "cxShadingWidget.h"
#include "cxDataViewSelectionWidget.h"
#include "sscTypeConversions.h"
#include "cxShadingParamsInterfaces.h"

namespace cx
{

//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------

DoubleDataAdapterImageTFDataBase::DoubleDataAdapterImageTFDataBase()
{
}

void DoubleDataAdapterImageTFDataBase::setImageTFData(ssc::ImageTFDataPtr tfData)
{
  if (mImageTFData)
    disconnect(mImageTFData.get(), SIGNAL(changed()), this, SIGNAL(changed()));

  mImageTFData = tfData;

  if (mImageTFData)
    connect(mImageTFData.get(), SIGNAL(changed()), this, SIGNAL(changed()));

  emit changed();
}

double DoubleDataAdapterImageTFDataBase::getValue() const
{
  if (!mImageTFData)
    return 0.0;
  return this->getValueInternal();
}

bool DoubleDataAdapterImageTFDataBase::setValue(double val)
{
  if (!mImageTFData)
    return false;
  this->setValueInternal(val);
  return true;
}

//---------------------------------------------------------
//---------------------------------------------------------

double DoubleDataAdapterImageTFDataWindow::getValueInternal() const
{
  return mImageTFData->getWindow();
}

void DoubleDataAdapterImageTFDataWindow::setValueInternal(double val)
{
  mImageTFData->setWindow(val);
}

ssc::DoubleRange DoubleDataAdapterImageTFDataWindow::getValueRange() const
{
  if (!mImageTFData)
    return ssc::DoubleRange();
  double range = mImageTFData->getScalarMax()-mImageTFData->getScalarMin();
  return ssc::DoubleRange(1,range,range/1000.0);
}

//---------------------------------------------------------
//---------------------------------------------------------

double DoubleDataAdapterImageTFDataLevel::getValueInternal() const
{
  return mImageTFData->getLevel();
}

void DoubleDataAdapterImageTFDataLevel::setValueInternal(double val)
{
  mImageTFData->setLevel(val);
}

ssc::DoubleRange DoubleDataAdapterImageTFDataLevel::getValueRange() const
{
  if (!mImageTFData)
    return ssc::DoubleRange();

  double max = mImageTFData->getScalarMax();
  double min = mImageTFData->getScalarMin();
  return ssc::DoubleRange(min,max,1);
}

//---------------------------------------------------------
//---------------------------------------------------------

double DoubleDataAdapterImageTFDataLLR::getValueInternal() const
{
  return mImageTFData->getLLR();
}
void DoubleDataAdapterImageTFDataLLR::setValueInternal(double val)
{
  mImageTFData->setLLR(val);
}
ssc::DoubleRange DoubleDataAdapterImageTFDataLLR::getValueRange() const
{
  if (!mImageTFData)
    return ssc::DoubleRange();

  double max = mImageTFData->getScalarMax();
  double min = mImageTFData->getScalarMin();
  return ssc::DoubleRange(min,max,(max-min)/1000.0);
}

//---------------------------------------------------------
//---------------------------------------------------------

double DoubleDataAdapterImageTFDataAlpha::getValueInternal() const
{
  return mImageTFData->getAlpha();
}
void DoubleDataAdapterImageTFDataAlpha::setValueInternal(double val)
{
  mImageTFData->setAlpha(val);
}
ssc::DoubleRange DoubleDataAdapterImageTFDataAlpha::getValueRange() const
{
  if (!mImageTFData)
    return ssc::DoubleRange();

  double max = 1.0;
  return ssc::DoubleRange(0,max,max/100.0);
}

//---------------------------------------------------------
//---------------------------------------------------------

TransferFunction3DWidget::TransferFunction3DWidget(QWidget* parent) :
  BaseWidget(parent, "TransferFunction3DWidget", "3D"),
  mLayout(new QVBoxLayout(this))
{
  mTransferFunctionAlphaWidget = new TransferFunctionAlphaWidget(this);
  mTransferFunctionColorWidget = new TransferFunctionColorWidget(this);

  mDataWindow.reset(new DoubleDataAdapterImageTFDataWindow);
  mDataLevel.reset(new DoubleDataAdapterImageTFDataLevel);
  mDataAlpha.reset(new DoubleDataAdapterImageTFDataAlpha);
  mDataLLR.reset(new DoubleDataAdapterImageTFDataLLR);

  mActiveImageProxy = ActiveImageProxy::New();
  connect(mActiveImageProxy.get(), SIGNAL(activeImageChanged(QString)), this, SLOT(activeImageChangedSlot()));
  connect(mActiveImageProxy.get(), SIGNAL(transferFunctionsChanged()), this, SLOT(activeImageChangedSlot()));

  mTransferFunctionAlphaWidget->setSizePolicy(QSizePolicy::MinimumExpanding,
                                              QSizePolicy::MinimumExpanding);
  mTransferFunctionColorWidget->setSizePolicy(QSizePolicy::Expanding,
                                              QSizePolicy::Fixed);

  mLayout->addWidget(mTransferFunctionAlphaWidget);
  mLayout->addWidget(mTransferFunctionColorWidget);

  QGridLayout* gridLayout = new QGridLayout;
  mLayout->addLayout(gridLayout);
  new ssc::SliderGroupWidget(this, mDataWindow, gridLayout, 0);
  new ssc::SliderGroupWidget(this, mDataLevel,  gridLayout, 1);
  new ssc::SliderGroupWidget(this, mDataAlpha,  gridLayout, 2);
  new ssc::SliderGroupWidget(this, mDataLLR,    gridLayout, 3);

  this->setLayout(mLayout);
}

QString TransferFunction3DWidget::defaultWhatsThis() const
{
  return "<html>"
    "<h3>3D Transfer Function</h3>"
    "<p>Lets you set a transfer function on a 3D volume.</p>"
    "<p><i></i></p>"
    "</html>";
}

void TransferFunction3DWidget::activeImageChangedSlot()
{
  ssc::ImagePtr image = ssc::dataManager()->getActiveImage();
  ssc::ImageTFDataPtr tf;
  if (image)
    tf = image->getTransferFunctions3D();
  else
    image.reset();

  mTransferFunctionAlphaWidget->setData(image, tf);
  mTransferFunctionColorWidget->setData(image, tf);

  mDataWindow->setImageTFData(tf);
  mDataLevel->setImageTFData(tf);
  mDataAlpha->setImageTFData(tf);
  mDataLLR->setImageTFData(tf);
}

//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------

TransferFunction2DWidget::TransferFunction2DWidget(QWidget* parent) :
  BaseWidget(parent, "TransferFunction2DWidget", "2D"),
  mLayout(new QVBoxLayout(this))
{
  mTransferFunctionAlphaWidget = new TransferFunctionAlphaWidget(this);
  mTransferFunctionAlphaWidget->setReadOnly(true);
  mTransferFunctionColorWidget = new TransferFunctionColorWidget(this);

  mDataWindow.reset(new DoubleDataAdapterImageTFDataWindow);
  mDataLevel.reset(new DoubleDataAdapterImageTFDataLevel);
  mDataAlpha.reset(new DoubleDataAdapterImageTFDataAlpha);
  mDataLLR.reset(new DoubleDataAdapterImageTFDataLLR);

  mActiveImageProxy = ActiveImageProxy::New();
  connect(mActiveImageProxy.get(), SIGNAL(activeImageChanged(QString)), this, SLOT(activeImageChangedSlot()));
  connect(mActiveImageProxy.get(), SIGNAL(transferFunctionsChanged()), this, SLOT(activeImageChangedSlot()));

  mTransferFunctionAlphaWidget->setSizePolicy(QSizePolicy::MinimumExpanding,
                                              QSizePolicy::MinimumExpanding);
  mTransferFunctionColorWidget->setSizePolicy(QSizePolicy::Expanding,
                                              QSizePolicy::Fixed);

  mLayout->addWidget(mTransferFunctionAlphaWidget);
  mLayout->addWidget(mTransferFunctionColorWidget);

  QGridLayout* gridLayout = new QGridLayout;
  mLayout->addLayout(gridLayout);
  new ssc::SliderGroupWidget(this, mDataWindow, gridLayout, 0);
  new ssc::SliderGroupWidget(this, mDataLevel,  gridLayout, 1);
  new ssc::SliderGroupWidget(this, mDataAlpha,  gridLayout, 2);
  new ssc::SliderGroupWidget(this, mDataLLR,    gridLayout, 3);

  this->setLayout(mLayout);
}

QString TransferFunction2DWidget::defaultWhatsThis() const
{
  return "<html>"
    "<h3>2D Transfer Function</h3>"
    "<p>Lets you set a transfer function on a 2D image.</p>"
    "<p><i></i></p>"
    "</html>";
}

void TransferFunction2DWidget::activeImageChangedSlot()
{
  ssc::ImagePtr image = ssc::dataManager()->getActiveImage();
  ssc::ImageTFDataPtr tf;
  if (image)
    tf = image->getLookupTable2D();
  else
    image.reset();

  mTransferFunctionAlphaWidget->setData(image, tf);
  mTransferFunctionColorWidget->setData(image, tf);

  mDataWindow->setImageTFData(tf);
  mDataLevel->setImageTFData(tf);
  mDataAlpha->setImageTFData(tf);
  mDataLLR->setImageTFData(tf);
}

//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------

TransferFunctionPresetWidget::TransferFunctionPresetWidget(QWidget* parent) :
  BaseWidget(parent, "TransferFunctionPresetWidget", "Transfer Function Presets"),
  mLayout(new QVBoxLayout(this))
{
	mPresets = ssc::dataManager()->getPresetTransferFunctions3D();
  QPushButton* resetButton = new QPushButton("Reset", this);
  resetButton->setToolTip("Reset all transfer function values to the defaults");
  connect(resetButton, SIGNAL(clicked()), this, SLOT(resetSlot()));

  QPushButton* deleteButton = new QPushButton("Delete", this);
  deleteButton->setToolTip("Delete the current preset");
  connect(deleteButton, SIGNAL(clicked()), this, SLOT(deleteSlot()));

  QPushButton* saveButton = new QPushButton("Save", this);
  saveButton->setToolTip("Create a new custom preset for both 2D and 3D");
  connect(saveButton, SIGNAL(clicked()), this, SLOT(saveSlot()));

  mPresetsComboBox = new QComboBox(this);
  mPresetsComboBox->setToolTip("Select a preset to use for both 2D and 3D");
  connect(mPresetsComboBox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(presetsBoxChangedSlot(const QString&)));

  mActiveImageProxy = ActiveImageProxy::New();
  connect(mActiveImageProxy.get(), SIGNAL(activeImageChanged(QString)), this, SLOT(generatePresetListSlot()));
  connect(mActiveImageProxy.get(), SIGNAL(propertiesChanged()), this, SLOT(generatePresetListSlot()));
  connect(mPresets.get(), SIGNAL(changed()), this, SLOT(generatePresetListSlot()));

  mLayout->addWidget(mPresetsComboBox);
  QHBoxLayout* buttonLayout = new QHBoxLayout;
  mLayout->addLayout(buttonLayout);

  buttonLayout->addWidget(resetButton);
  buttonLayout->addWidget(deleteButton);
  buttonLayout->addWidget(saveButton);

  this->setLayout(mLayout);
}

QString TransferFunctionPresetWidget::defaultWhatsThis() const
{
  return "<html>"
    "<h3>Transfer Function Presets</h3>"
    "<p>Lets you select a predefined transfer function.</p>"
    "<p><i></i></p>"
    "</html>";
}

void TransferFunctionPresetWidget::generatePresetListSlot()
{
	// Re-initialize the list
  mPresetsComboBox->blockSignals(true);
  mPresetsComboBox->clear();

  mPresetsComboBox->addItem("Transfer function preset...");

  if (ssc::dataManager()->getActiveImage())
  	mPresetsComboBox->addItems(mPresets->getPresetList(ssc::dataManager()->getActiveImage()->getModality()));
  else //No active image, show all available presets for debug/overview purposes
  	mPresetsComboBox->addItems(mPresets->getPresetList("UNKNOWN"));
  mPresetsComboBox->blockSignals(false);
}

void TransferFunctionPresetWidget::presetsBoxChangedSlot(const QString& presetName)
{
  ssc::ImagePtr activeImage = ssc::dataManager()->getActiveImage();
  if(activeImage)
    mPresets->load(presetName, activeImage);
}

void TransferFunctionPresetWidget::resetSlot()
{
  ssc::ImagePtr activeImage = ssc::dataManager()->getActiveImage();
  activeImage->resetTransferFunctions();
  mPresetsComboBox->setCurrentIndex(0);
}

void TransferFunctionPresetWidget::saveSlot()
{
	// generate a name suggestion: identical if custom, appended by index if default.
	QString newName = mPresetsComboBox->currentText();
//	std::cout << "list:  " << qstring_cast(mPresets->getPresetList("").join("--")) << std::endl;
	if (!mPresets->getPresetList("").contains(newName))
		newName = "custom preset";
	if (mPresets->isDefaultPreset(newName))
		newName += "(2)";

    bool ok;
    QString text = QInputDialog::getText(this, "Save Preset",
                                         "Custom Preset Name", QLineEdit::Normal,
                                         newName, &ok);
    if (!ok || text.isEmpty())
      return;

  ssc::ImagePtr activeImage = ssc::dataManager()->getActiveImage();
  mPresets->save(text, activeImage);

  this->generatePresetListSlot();
  mPresetsComboBox->setCurrentIndex(mPresetsComboBox->findText(text));
}


void TransferFunctionPresetWidget::deleteSlot()
{
	if (mPresets->isDefaultPreset(mPresetsComboBox->currentText()))
	{
		ssc::messageManager()->sendWarning("It is not possible to delete one of the default presets");
		return;
	}
	if (QMessageBox::question(this, "Delete current preset", "Do you really want to delete the current preset?",
			QMessageBox::Cancel | QMessageBox::Ok) != QMessageBox::Ok)
			return;
	mPresets->deletePresetData(mPresetsComboBox->currentText());

  this->generatePresetListSlot();
  this->resetSlot();
}

//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------

TransferFunctionWidget::TransferFunctionWidget(QWidget* parent) :
  BaseWidget(parent, "TransferFunctionWidget", "Transfer Function")
{
  QVBoxLayout* mLayout = new QVBoxLayout(this);

  mLayout->setMargin(0);
  mLayout->addWidget(new TransferFunction3DWidget(this));
  mLayout->addWidget(new TransferFunctionPresetWidget(this));

  this->setLayout(mLayout);
}

QString TransferFunctionWidget::defaultWhatsThis() const
{
  return "<html>"
    "<h3>Transfer Function.</h3>"
    "<p>Lets you set a new or predefined transfer function on a volume.</p>"
    "<p><i></i></p>"
    "</html>";
}
}//namespace cx
