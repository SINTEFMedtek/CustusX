/*
 *  sscReconstructionWidget.cpp
 *  Created by Ole Vegard Solberg on 5/4/10.
 */
#include "sscReconstructionWidget.h"
#include "sscTypeConversions.h"
#include "sscReconstructOutputValueParamsInterfaces.h"
#include "sscMessageManager.h"

namespace ssc 
{


// --------------------------------------------------------
// --------------------------------------------------------
// --------------------------------------------------------


ReconstructionWidget::ReconstructionWidget(QWidget* parent, QString appDataPath, QString shaderPath):
  QWidget(parent),
  mReconstructer(new Reconstructer(appDataPath, shaderPath))
{
  this->setWindowTitle("US Reconstruction");

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
  mMaxVolSizeWidget = new ssc::SliderGroupWidget(this, ssc::DoubleDataAdapterPtr(new DoubleDataAdapterMaxUSVolumeSize(mReconstructer)), outputVolGridLayout, 0);
  mSpacingWidget = new ssc::SliderGroupWidget(this, ssc::DoubleDataAdapterPtr(new DoubleDataAdapterSpacing(mReconstructer)), outputVolGridLayout, 1);
  mDimXWidget = new ssc::SliderGroupWidget(this, ssc::DoubleDataAdapterPtr(new DoubleDataAdapterXDim(mReconstructer)), outputVolGridLayout, 2);
  mDimYWidget = new ssc::SliderGroupWidget(this, ssc::DoubleDataAdapterPtr(new DoubleDataAdapterYDim(mReconstructer)), outputVolGridLayout, 3);
  mDimZWidget = new ssc::SliderGroupWidget(this, ssc::DoubleDataAdapterPtr(new DoubleDataAdapterZDim(mReconstructer)), outputVolGridLayout, 4);

  ssc::StringDataAdapterPtr orientation = this->generateStringDataAdapter("Orientation");
  ssc::ComboGroupWidget* orientationWidget = new ssc::ComboGroupWidget(this, orientation);

  ssc::StringDataAdapterPtr algorithm = this->generateStringDataAdapter("Algorithm");
  ssc::ComboGroupWidget* algorithmWidget = new ssc::ComboGroupWidget(this, algorithm);

  QGroupBox* algorithmGroup = new QGroupBox("Algorithm", this);
  QGridLayout* algoLayout = new QGridLayout(algorithmGroup);

  QDomNodeList algoSettings = mReconstructer->getSettings().namedItem("algorithms").namedItem("ThunderVNN").childNodes();
  for (int i=0; i<algoSettings.size(); ++i)
  {
    StringOptionItem item(algoSettings.item(i).toElement());
    ssc::StringDataAdapterXmlNodePtr interface(new StringDataAdapterXmlNode(item));
    connect(interface.get(), SIGNAL(valueWasSet()), mReconstructer.get(), SLOT(setSettings()));
    connect(mReconstructer.get(), SIGNAL(paramsChanged()), interface.get(), SIGNAL(changed()));
    ssc::ComboGroupWidget* widget = new ssc::ComboGroupWidget(this, interface, algoLayout, i);
    widget = widget;
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

  //this->selectData(defPath+defFile);
}

ssc::StringDataAdapterPtr ReconstructionWidget::generateStringDataAdapter(QString uid)
{
  StringOptionItem item = StringOptionItem::fromName(uid, mReconstructer->getSettings());

  ssc::StringDataAdapterXmlNodePtr interface(new StringDataAdapterXmlNode(item));
  connect(interface.get(), SIGNAL(valueWasSet()), mReconstructer.get(), SLOT(setSettings()));
  connect(mReconstructer.get(), SIGNAL(paramsChanged()), interface.get(), SIGNAL(changed()));
  //ssc::ComboGroupWidget* widget = new ssc::ComboGroupWidget(this, interface, algoLayout, 5+i);
  return interface;
}


void ReconstructionWidget::currentDataComboIndexChanged(const QString& text)
{
  QDir dir = QFileInfo(mInputFile).dir();
  this->selectData(dir.filePath(text));
}

QString ReconstructionWidget::getCurrentPath()
{
  return QFileInfo(mInputFile).dir().absolutePath();
}

void ReconstructionWidget::reconstruct()
{
  ssc::messageManager()->sendInfo("Reconstructing...");
  qApp->processEvents();
  mReconstructer->reconstruct();
}

void ReconstructionWidget::updateComboBox()
{
  mDataComboBox->blockSignals(true);
  mDataComboBox->clear();

  QDir dir = QFileInfo(mInputFile).dir();
  QStringList nameFilters;
  nameFilters << "*.mhd";
  QStringList files = dir.entryList(nameFilters, QDir::Files);

  for (int i=0; i<files.size(); ++i)
  {
    mDataComboBox->addItem(files[i]);
  }
  mDataComboBox->setCurrentIndex(-1);
  for (int i=0; i<files.size(); ++i)
  {
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
    ssc::messageManager()->sendWarning("no file selected");
    return;
  }

  mInputFile = filename;

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
