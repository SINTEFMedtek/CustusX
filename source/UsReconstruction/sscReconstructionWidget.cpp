/*
 *  sscReconstructionWidget.cpp
 *  Created by Ole Vegard Solberg on 5/4/10.
 */
#include "sscReconstructionWidget.h"
#include "sscTypeConversions.h"
#include "sscReconstructOutputValueParamsInterfaces.h"

namespace ssc 
{


ComboGroupWidget::ComboGroupWidget(QWidget* parent, ssc::StringDataInterfacePtr dataInterface, QGridLayout* gridLayout, int row)
{
  mData = dataInterface;
  connect(mData.get(), SIGNAL(changed()), this, SLOT(dataChanged()));

  QHBoxLayout* topLayout = new QHBoxLayout;
  topLayout->setMargin(0);
  this->setLayout(topLayout);

  mLabel = new QLabel(this);
  mLabel->setText(mData->getValueName());
  topLayout->addWidget(mLabel);

  mCombo = new QComboBox(this);
  topLayout->addWidget(mCombo);
  connect(mCombo, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(comboIndexChanged(const QString&)));

  if (gridLayout) // add to input gridlayout
  {
    gridLayout->addWidget(mLabel,  row, 0);
    gridLayout->addWidget(mCombo,  row, 1);
  }
  else // add directly to this
  {
    topLayout->addWidget(mLabel);
    topLayout->addWidget(mCombo);
  }

  dataChanged();
}

void ComboGroupWidget::comboIndexChanged(const QString& val)
{
  mData->setValue(val);
}

//void ComboGroupWidget::doubleValueChanged(double val)
//{
//  val = mData->convertDisplay2Internal(val);
//
//  if (ssc::similar(val, mData->getValue()))
//      return;
//
//  mData->setValue(val);
//}
//
//void ComboGroupWidget::textEditedSlot(const QString& text)
//{
//  double defVal =mData->convertInternal2Display(mData->getValue()); // defval in display space
//  double newVal = mData->convertDisplay2Internal(mEdit->getDoubleValue(defVal)); // newval iin internal space
//
//  if (ssc::similar(newVal, mData->getValue()))
//      return;
//
//  mData->setValue(newVal);
//}

void ComboGroupWidget::dataChanged()
{
  mCombo->blockSignals(true);
  mCombo->clear();

  QString currentValue = mData->getValue();
  //std::cout << "datachanged for " << mData->getValueID() << "=" << currentValue << std::endl;
  QStringList range = mData->getValueRange();
  for (int i=0; i<range.size(); ++i)
  {
    //std::cout << range[i] << std::endl;
    mCombo->addItem(range[i]);
    if (range[i]==currentValue)
      mCombo->setCurrentIndex(i);
  }

  mCombo->setToolTip(mData->getHelp());
  mCombo->blockSignals(false);
}


// --------------------------------------------------------
// --------------------------------------------------------
// --------------------------------------------------------


ReconstructionWidget::ReconstructionWidget(QWidget* parent):
  QWidget(parent),
  mReconstructer(new Reconstructer())
{
  this->setWindowTitle("US Reconstruction");

/*#define input_set_mac "/Users/olevs/data/UL_thunder/Nevro_Spine/SpineData/ultrasoundSample5.mhd", \
"/Users/olevs/data/UL_thunder/Nevro_Spine/SpineData/ultrasoundSample5.pos", \
"/Users/olevs/data/UL_thunder/Nevro_Spine/SpineData/ultrasoundSample5.tim", \
"/Users/olevs/data/UL_thunder/Nevro_Spine/SpineData/ultrasoundSample5.msk", \
"/Users/olevs/data/UL_thunder/Nevro_Spine/SpineData/ultrasoundSample5.vol", \
"/Users/olevs/data/UL_thunder/Nevro_Spine/calibration_files/M12L.cal"
  //#define input_set_mac_origo_x 850.0f
  //#define input_set_mac_origo_y 10.0f
  //#define input_set_mac_origo_z 2800.0f
#define input_set_mac_origo_x 85.0f
#define input_set_mac_origo_y 1.0f
#define input_set_mac_origo_z 280.0f*/

//#define CA_DEFS

#ifdef CA_DEFS
  QString defPath = "/Users/christiana/workspace/sessions/us_acq_holger_data/";
  QString defFile = "ultrasoundSample5.mhd";
#else
  //QString defPath = "/Users/olevs/data/UL_thunder/test/1/";
  //QString defFile = "UsAcq_1.mhd";

  QString defPath = "/Users/olevs/data/UL_thunder/test/coordinateSys_test/";
  //QString defPath = "/Users/olevs/data/UL_thunder/test/";
  QString defFile = "USAcq_29.mhd";
#endif

  //mInputFile = path + "UsAcq_1.mhd";
  //mInputFile = path + "ultrasoundSample5.mhd";
  connect(mReconstructer.get(), SIGNAL(paramsChanged()), this, SLOT(paramsChangedSlot()));

  QVBoxLayout* topLayout = new QVBoxLayout(this);

  QHBoxLayout* dataLayout = new QHBoxLayout;
  mDataComboBox = new QComboBox(this);
  connect(mDataComboBox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(currentDataComboIndexChanged(const QString&)));
  //this->updateComboBox();

  mSelectDataAction = new QAction(QIcon(":/icons/open.png"), tr("&Select data"), this);
  connect(mSelectDataAction, SIGNAL(triggered()), this, SLOT(selectData()));
  mSelectDataButton = new QToolButton(this);
  mSelectDataButton->setDefaultAction(mSelectDataAction);

  QHBoxLayout* extentLayout = new QHBoxLayout;
  mExtentLineEdit = new QLineEdit(this);
  mExtentLineEdit->setReadOnly(true);
  extentLayout->addWidget(new QLabel("Extent(mm)", this));
  extentLayout->addWidget(mExtentLineEdit);

  QHBoxLayout* inputSpacingLayout = new QHBoxLayout;
  mInputSpacingLineEdit = new QLineEdit(this);
  mInputSpacingLineEdit->setReadOnly(true);
  inputSpacingLayout->addWidget(new QLabel("Input Spacing(mm)", this));
  inputSpacingLayout->addWidget(mInputSpacingLineEdit);

//  mReloadButton = new QPushButton("Reload", this);
//  connect(mReloadButton, SIGNAL(clicked()), this, SLOT(reload()));
  mReconstructButton = new QPushButton("Reconstruct", this);
  connect(mReconstructButton, SIGNAL(clicked()), this, SLOT(reconstruct()));

  QGroupBox* outputVolGroup = new QGroupBox("Output Volume", this);
  QVBoxLayout* outputVolLayout = new QVBoxLayout(outputVolGroup);

  QGridLayout* outputVolGridLayout = new QGridLayout;
  mMaxVolSizeWidget = new ssc::SliderGroupWidget(this, ssc::DoubleDataInterfacePtr(new DoubleDataInterfaceMaxUSVolumeSize(mReconstructer)), outputVolGridLayout, 0);
  mSpacingWidget = new ssc::SliderGroupWidget(this, ssc::DoubleDataInterfacePtr(new DoubleDataInterfaceSpacing(mReconstructer)), outputVolGridLayout, 1);
  mDimXWidget = new ssc::SliderGroupWidget(this, ssc::DoubleDataInterfacePtr(new DoubleDataInterfaceXDim(mReconstructer)), outputVolGridLayout, 2);
  mDimYWidget = new ssc::SliderGroupWidget(this, ssc::DoubleDataInterfacePtr(new DoubleDataInterfaceYDim(mReconstructer)), outputVolGridLayout, 3);
  mDimZWidget = new ssc::SliderGroupWidget(this, ssc::DoubleDataInterfacePtr(new DoubleDataInterfaceZDim(mReconstructer)), outputVolGridLayout, 4);

  ssc::StringDataInterfacePtr orientation = this->generateStringDataInterface("Orientation");
  ssc::ComboGroupWidget* orientationWidget = new ssc::ComboGroupWidget(this, orientation);

  ssc::StringDataInterfacePtr algorithm = this->generateStringDataInterface("Algorithm");
  ssc::ComboGroupWidget* algorithmWidget = new ssc::ComboGroupWidget(this, algorithm);

  QGroupBox* algorithmGroup = new QGroupBox("Algorithm", this);
  QGridLayout* algoLayout = new QGridLayout(algorithmGroup);

  QDomNodeList algoSettings = mReconstructer->getSettings().namedItem("algorithms").namedItem("ThunderVNN").childNodes();
  for (int i=0; i<algoSettings.size(); ++i)
  {
    StringOptionItem item(algoSettings.item(i).toElement());
    ssc::StringDataInterfaceXmlNodePtr interface(new StringDataInterfaceXmlNode(item));
    connect(interface.get(), SIGNAL(valueWasSet()), mReconstructer.get(), SLOT(setSettings()));
    connect(mReconstructer.get(), SIGNAL(paramsChanged()), interface.get(), SIGNAL(changed()));
    ssc::ComboGroupWidget* widget = new ssc::ComboGroupWidget(this, interface, algoLayout, i);
  }

  topLayout->addLayout(dataLayout);
    dataLayout->addWidget(mDataComboBox);
    dataLayout->addWidget(mSelectDataButton);
  //topLayout->addWidget(mReloadButton);
  topLayout->addWidget(outputVolGroup);
    outputVolLayout->addLayout(extentLayout);
    outputVolLayout->addLayout(inputSpacingLayout);
    outputVolLayout->addLayout(outputVolGridLayout);
    outputVolLayout->addWidget(orientationWidget);
  topLayout->addWidget(algorithmWidget);
  topLayout->addWidget(algorithmGroup);
  topLayout->addWidget(mReconstructButton);
  topLayout->addStretch();

  this->selectData(defPath+defFile);
}

ssc::StringDataInterfacePtr ReconstructionWidget::generateStringDataInterface(QString uid)
{
  StringOptionItem item = StringOptionItem::fromName(uid, mReconstructer->getSettings());

  ssc::StringDataInterfaceXmlNodePtr interface(new StringDataInterfaceXmlNode(item));
  connect(interface.get(), SIGNAL(valueWasSet()), mReconstructer.get(), SLOT(setSettings()));
  connect(mReconstructer.get(), SIGNAL(paramsChanged()), interface.get(), SIGNAL(changed()));
  //ssc::ComboGroupWidget* widget = new ssc::ComboGroupWidget(this, interface, algoLayout, 5+i);
  return interface;
}


void ReconstructionWidget::currentDataComboIndexChanged(const QString& text)
{
  QDir dir = QFileInfo(mInputFile).dir();
  this->selectData(dir.filePath(text));
//  std::cout << "selected: " << mInputFile << std::endl;
//  mDataComboBox->setToolTip(mInputFile);
}

QString ReconstructionWidget::getCurrentPath()
{
  return QFileInfo(mInputFile).dir().absolutePath();
}

void ReconstructionWidget::reconstruct()
{
  mReconstructer->reconstruct();
}

void ReconstructionWidget::updateComboBox()
{
  mDataComboBox->blockSignals(true);
  mDataComboBox->clear();

  QDir dir = QFileInfo(mInputFile).dir();
  QStringList nameFilters;
  nameFilters << "*.mhd";
  std::cout << dir.path() << std::endl;
  QStringList files = dir.entryList(nameFilters, QDir::Files);

  for (int i=0; i<files.size(); ++i)
  {
    std::cout << files[i] << std::endl;
    mDataComboBox->addItem(files[i]);
    if (files[i]==QFileInfo(mInputFile).fileName())
      mDataComboBox->setCurrentIndex(i);
  }

  mDataComboBox->setToolTip(mInputFile);

  //mDataComboBox->addItem(mInputFile);
  mDataComboBox->blockSignals(false);

}

void ReconstructionWidget::reload()
{
  this->selectData(mInputFile);
}

void ReconstructionWidget::selectData(QString filename)
{
  if(filename.isEmpty())
  {
    std::cout << "no file selected" << std::endl;
    return;
  }

  mInputFile = filename;

  std::cout << "selected: " << mInputFile << std::endl;
  this->updateComboBox();
  mDataComboBox->setToolTip(mInputFile);

  // read data into reconstructer

  QStringList list = mInputFile.split("/");
  list[list.size()-1] = "";
  QString calFilesPath = list.join("/")+"/";
  mReconstructer->readFiles(mInputFile, calFilesPath);
}

/** Called when parameters in the reconstructer has changed
 *
 */
void ReconstructionWidget::paramsChangedSlot()
{
  //  ssc::DoubleBoundingBox3D extent = mReconstructer->getExtent();
    ssc::Vector3D range = mReconstructer->getOutputVolumeParams().mExtent.range();

    QString extText = QString("%1,  %2,  %3").arg(range[0],0,'f',1).arg(range[1],0,'f',1).arg(range[2],0,'f',1);
    mExtentLineEdit->setText(extText);

    mInputSpacingLineEdit->setText(QString("%1").arg(mReconstructer->getOutputVolumeParams().mInputSpacing,0,'f',4));
}

void ReconstructionWidget::selectData()
{
  QString filename = QFileDialog::getOpenFileName( this,
                                  QString(tr("Select data file")),
                                  getCurrentPath(),
                                  tr("USAcq (*.mhd)"));
  this->selectData(filename);
}
  
}//namespace
