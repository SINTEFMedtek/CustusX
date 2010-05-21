/*
 *  sscReconstructionWidget.cpp
 *  Created by Ole Vegard Solberg on 5/4/10.
 */


#include "sscReconstructionWidget.h"
#include "sscTypeConversions.h"
#include "cxDataInterface.h"

namespace ssc 
{


DoubleDataInterfaceMaxUSVolumeSize::DoubleDataInterfaceMaxUSVolumeSize(ReconstructerPtr reconstructer) :
    mFactor(1024*1024), mReconstructer(reconstructer)
{
  //connect(toolManager(), SIGNAL(dominantToolChanged(const std::string&)), this, SLOT(dominantToolChangedSlot()));
  //dominantToolChangedSlot();
}

double DoubleDataInterfaceMaxUSVolumeSize::getValue() const
{
  return mReconstructer->getMaxOutputVolumeSize();
}

bool DoubleDataInterfaceMaxUSVolumeSize::setValue(double val)
{
  if (similar(val, mReconstructer->getMaxOutputVolumeSize()))
    return false;

  mReconstructer->setMaxOutputVolumeSize(val);
  emit changed();
  return true;
}

ssc::DoubleRange DoubleDataInterfaceMaxUSVolumeSize::getValueRange() const
{
  return ssc::DoubleRange(mFactor,mFactor*500,mFactor);
}

//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------


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
  
  QString defPath = "/Users/olevs/data/UL_thunder/test/1/";
  QString defFile = "UsAcq_1.mhd";
  //QString defPath = "/Users/christiana/workspace/sessions/us_acq_holger_data/";
  //QString defPath = "/Users/olevs/data/UL_thunder/test/";
  //QString defFile = "ultrasoundSample5.mhd";


  //mInputFile = path + "UsAcq_1.mhd";
  //mInputFile = path + "ultrasoundSample5.mhd";

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

  mReconstructButton = new QPushButton("Reconstruct", this);
  connect(mReconstructButton, SIGNAL(clicked()), this, SLOT(reconstruct()));

  QGridLayout* gridLayout = new QGridLayout;
  mMaxVolSizeWidget = new ssc::SliderGroupWidget(this, ssc::DoubleDataInterfacePtr(new DoubleDataInterfaceMaxUSVolumeSize(mReconstructer)), gridLayout, 0);

  topLayout->addLayout(dataLayout);
  dataLayout->addWidget(mDataComboBox);
  dataLayout->addWidget(mSelectDataButton);
  topLayout->addLayout(extentLayout);
  topLayout->addLayout(gridLayout);
  topLayout->addWidget(mReconstructButton);
  topLayout->addStretch();

  this->selectData(defPath+defFile);
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
  QString calFile = QFileInfo(mInputFile).dir().filePath("M12L.cal");

  //mReconstructer->reconstruct(mInputFile, calFile);
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
  QString calFile = QFileInfo(mInputFile).dir().filePath("M12L.cal");
  mReconstructer->readFiles(mInputFile, calFile);

//  ssc::DoubleBoundingBox3D extent = mReconstructer->getExtent();
  ssc::Vector3D range = mReconstructer->getExtent().range();

  QString extText = QString("%1,  %2,  %3").arg(range[0],0,'f',1).arg(range[1],0,'f',1).arg(range[2],0,'f',1);
  mExtentLineEdit->setText(extText);
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
