/*
 *  sscReconstructionWidget.cpp
 *  Created by Ole Vegard Solberg on 5/4/10.
 */


#include "sscReconstructionWidget.h"
#include "sscTypeConversions.h"

namespace ssc 
{
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
  
  //QString path = "/Users/olevs/data/UL_thunder/test/1/";
  QString path = "/Users/christiana/workspace/sessions/us_acq_holger_data/";

  //mInputFile = path + "UsAcq_1.mhd";
  mInputFile = path + "ultrasoundSample5.mhd";

  QVBoxLayout* topLayout = new QVBoxLayout(this);

  QHBoxLayout* dataLayout = new QHBoxLayout;
  mDataComboBox = new QComboBox(this);
  connect(mDataComboBox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(currentDataComboIndexChanged(const QString&)));
  this->updateComboBox();

  mSelectDataAction = new QAction(QIcon(":/icons/open.png"), tr("&Select data"), this);
  connect(mSelectDataAction, SIGNAL(triggered()), this, SLOT(selectData()));
  mSelectDataButton = new QToolButton(this);
  mSelectDataButton->setDefaultAction(mSelectDataAction);

  mReconstructButton = new QPushButton("Reconstruct", this);
  connect(mReconstructButton, SIGNAL(clicked()), this, SLOT(reconstruct()));

  topLayout->addLayout(dataLayout);
  dataLayout->addWidget(mDataComboBox);
  dataLayout->addWidget(mSelectDataButton);
  topLayout->addWidget(mReconstructButton);
  topLayout->addStretch();
}

void ReconstructionWidget::currentDataComboIndexChanged(const QString& text)
{
  QDir dir = QFileInfo(mInputFile).dir();
  mInputFile = dir.filePath(text);
  std::cout << "selected: " << mInputFile << std::endl;
  mDataComboBox->setToolTip(mInputFile);
}

QString ReconstructionWidget::getCurrentPath()
{
  return QFileInfo(mInputFile).dir().absolutePath();
}

void ReconstructionWidget::reconstruct()
{
  QString calFile = QFileInfo(mInputFile).dir().filePath("M12L.cal");

  mReconstructer->reconstruct(mInputFile, calFile);
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

void ReconstructionWidget::selectData()
{
  QString filename = QFileDialog::getOpenFileName( this,
                                  QString(tr("Select data file")),
                                  getCurrentPath(),
                                  tr("USAcq (*.mhd)"));
  if(filename.isEmpty())
  {
    std::cout << "no file selected" << std::endl;
    return;
  }

  mInputFile = filename;

  updateComboBox();
}
  
}//namespace
