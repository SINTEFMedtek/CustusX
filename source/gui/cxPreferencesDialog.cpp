
#include <QtGui>

#include "cxPreferencesDialog.h"
#include "cxMessageManager.h"
#include "cxViewManager.h"

#include <iostream>

namespace cx
{
FoldersTab::FoldersTab(QWidget *parent) :
  QWidget(parent),
  mSettings(new QSettings())
{}

void FoldersTab::init(){
  mGlobalPatientDataFolder = mSettings->value("globalPatientDataFolder").toString();
  //mCurrentImportDataFolder  = mSettings->value("mainWindow/importDataFolder").toString();
  mCurrentToolConfigFilePath = mSettings->value("toolConfigFilePath").toString();

  QFileInfo fileInfo( mCurrentToolConfigFilePath );
  mCurrentToolConfigFolder = fileInfo.absolutePath();
  mCurrentToolConfigFile = fileInfo.fileName();

  // patientDataFolder
  QLabel* patientDataFolderLabel = new QLabel(tr("Patient data folder:"));

  mPatientDataFolderComboBox = new QComboBox;
  mPatientDataFolderComboBox->addItem( mGlobalPatientDataFolder);
  //patientDataFolderComboBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

  QPushButton* patientDataFolderButton = new QPushButton(tr("B&rowse..."));
  connect( patientDataFolderButton, 
          SIGNAL(clicked()), 
          this, 
          SLOT(browsePatientDataFolderSlot()) );

  //QLabel *currentPatientLabel = new QLabel(tr("Current patient:"));
  //QComboBox *currentPatientComboBox = new QComboBox;
  
  // importDataFolder
  /*/QLabel* importDataFolderLabel = new QLabel(tr("Import data folder:"));
  
  mImportDataFolderComboBox = new QComboBox;
  mImportDataFolderComboBox->addItem( mCurrentImportDataFolder);
  //importDataFolderComboBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
  
  QPushButton* importDataFolderButton = new QPushButton(tr("&Browse..."));
  connect( importDataFolderButton, 
          SIGNAL(clicked()), 
          this, 
          SLOT(browseImportDataFolderSlot()) );*/
  

  // toolConfigFolder
  QLabel* toolConfigFolderLabel = new QLabel(tr("Tool configuration folder:"));

  mToolConfigFolderComboBox = new QComboBox;
  mToolConfigFolderComboBox->addItem( mCurrentToolConfigFolder );
  //mToolConfigFolderComboBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

  QPushButton* toolConfigFolderButton = new QPushButton(tr("&Browse..."));
  connect( toolConfigFolderButton, 
          SIGNAL(clicked()), 
          this, 
          SLOT(browseToolConfigFolderSlot()) );

  QLabel *toolConfigFilesLabel = new QLabel(tr("Tool configuration files:"));
  mToolConfigFilesComboBox = new QComboBox;
  setToolConfigFiles();
  connect( mToolConfigFilesComboBox, 
          SIGNAL(currentIndexChanged(const QString &)), 
          this, 
          SLOT(currentToolConfigFilesIndexChangedSlot(const QString &)) );
  setCurrentToolConfigFile();

  // Choose application name
  QLabel* chooseApplicationLabel = new QLabel(tr("Choose application name, used when creating new patients:"));
  mChooseApplicationComboBox = new QComboBox();
  setApplicationComboBox();
  connect(mChooseApplicationComboBox,
          SIGNAL(currentIndexChanged(const QString &)),
          this,
          SLOT(currenApplicationChangedSlot(const QString &)));
  setCurrentApplication();

  // Layout
  QGridLayout *mainLayout = new QGridLayout;
  mainLayout->addWidget(patientDataFolderLabel, 0, 0);
  mainLayout->addWidget(mPatientDataFolderComboBox, 0, 1);
  mainLayout->addWidget(patientDataFolderButton, 0, 2);
  
  //mainLayout->addWidget(importDataFolderLabel, 1, 0);
  //mainLayout->addWidget(mImportDataFolderComboBox, 1, 1);
  //mainLayout->addWidget(importDataFolderButton, 1, 2);
  
  //mainLayout->addWidget(currentPatientLabel, 2, 0);
  //mainLayout->addWidget(currentPatientComboBox, 3, 0);

  //mainLayout->addWidget(dummyLabel, 4, 0);
  //mainLayout->addWidget(dummyLabel, 5, 0);

  mainLayout->addWidget(toolConfigFolderLabel, 6, 0);
  mainLayout->addWidget(mToolConfigFolderComboBox, 6, 1);
  mainLayout->addWidget(toolConfigFolderButton, 6, 2);
  mainLayout->addWidget(toolConfigFilesLabel, 8, 0);
  mainLayout->addWidget(mToolConfigFilesComboBox, 8, 1);
  
  mainLayout->addWidget(chooseApplicationLabel, 9, 0);
  mainLayout->addWidget(mChooseApplicationComboBox, 9, 1);
 
  setLayout(mainLayout);
  
}

FoldersTab::~FoldersTab()
{}

void FoldersTab::browsePatientDataFolderSlot()
{
  mGlobalPatientDataFolder = QFileDialog::getExistingDirectory(this, 
                                                     tr("Find Patient Data Folder"), 
                                                     mGlobalPatientDataFolder,
                                                     QFileDialog::ShowDirsOnly);
  if( !mGlobalPatientDataFolder.isEmpty() ) {
    mPatientDataFolderComboBox->addItem( mGlobalPatientDataFolder );
    mPatientDataFolderComboBox->setCurrentIndex( mPatientDataFolderComboBox->currentIndex() + 1 );
  }
}
  
/*void FoldersTab::browseImportDataFolderSlot()
{
  mCurrentImportDataFolder = QFileDialog::getExistingDirectory(this, 
                                                     tr("Find Patient Data Folder"), 
                                                     mCurrentImportDataFolder,
                                                     QFileDialog::ShowDirsOnly);
  if( !mCurrentImportDataFolder.isEmpty() ) {
    mImportDataFolderComboBox->addItem( mCurrentImportDataFolder );
    mImportDataFolderComboBox->setCurrentIndex( mImportDataFolderComboBox->currentIndex() + 1 );
  }
}*/

void FoldersTab::browseToolConfigFolderSlot()
{
  mCurrentToolConfigFolder = QFileDialog::getExistingDirectory(this, 
                                                     tr("Find Tool Config Folder"), 
                                                     mCurrentToolConfigFolder,
                                                     QFileDialog::ShowDirsOnly);
  if( !mCurrentToolConfigFolder.isEmpty() ) {
    mToolConfigFolderComboBox->addItem( mCurrentToolConfigFolder );
    mToolConfigFolderComboBox->setCurrentIndex( mToolConfigFolderComboBox->currentIndex() + 1 );
    setToolConfigFiles();
  }

}

void FoldersTab::currentToolConfigFilesIndexChangedSlot(const QString & newToolConfigFile)
{
  mCurrentToolConfigFile = newToolConfigFile;
}

void FoldersTab::currenApplicationChangedSlot(const QString & newApplicationName)
{
  mSettings->setValue("globalApplicationName", newApplicationName);
}

void FoldersTab::setToolConfigFiles()
{
	QDir dir(mCurrentToolConfigFolder);
    dir.setFilter(QDir::Files);

    QStringList nameFilters;
    nameFilters << "*.xml";
    dir.setNameFilters(nameFilters);

    QStringList list = dir.entryList();
	
    mToolConfigFilesComboBox->clear();
    mToolConfigFilesComboBox->addItems( list );
}

void FoldersTab::setApplicationComboBox()
{
  QString str = mSettings->value("applicationNames").toString();
  QStringList list = str.split(",");
  mChooseApplicationComboBox->clear();
  mChooseApplicationComboBox->addItems(list);
}

void FoldersTab::setCurrentToolConfigFile()
{
  int currentIndex = mToolConfigFilesComboBox->findText( mCurrentToolConfigFile );
  mToolConfigFilesComboBox->setCurrentIndex( currentIndex );
}
  
  
void FoldersTab::setCurrentApplication()
{
  int currentIndex = mChooseApplicationComboBox->findText( mSettings->value("globalApplicationName").toString() );
  mChooseApplicationComboBox->setCurrentIndex( currentIndex );
}
  
void FoldersTab::saveParametersSlot()
{
  
  // currentPatientDataFolder
  mSettings->setValue("globalPatientDataFolder", mGlobalPatientDataFolder);
  //mSettings->setValue("mainWindow/importDataFolder",  mCurrentImportDataFolder);
  
  // currentToolConfigFilePath
  QDir dir(mCurrentToolConfigFolder);
  mCurrentToolConfigFilePath = dir.absoluteFilePath( mCurrentToolConfigFile );
  mSettings->setValue("toolConfigFilePath", mCurrentToolConfigFilePath);
  
  mSettings->sync();
  emit savedParameters();
}
  
//==============================================================================
// PerformanceTab
//------------------------------------------------------------------------------
PerformanceTab::PerformanceTab(QWidget *parent) :
  QWidget(parent),
  mSettings(new QSettings())
{
}

void PerformanceTab::init()
{
  int renderingInterval = mSettings->value("renderingInterval").toInt();
  bool shadingOn = mSettings->value("shadingOn").toBool();
  
  QLabel* renderingIntervalLabel = new QLabel(tr("Rendering interval"));
  
  mRenderingIntervalSpinBox = new QSpinBox;
  mRenderingIntervalSpinBox->setSuffix("ms");
  mRenderingIntervalSpinBox->setMaximum(1);
  mRenderingIntervalSpinBox->setMaximum(1000);
  mRenderingIntervalSpinBox->setValue(renderingInterval);
  connect(mRenderingIntervalSpinBox,
          SIGNAL(valueChanged(int)),
          this,
          SLOT(renderingIntervalSlot(int)));
  
  QString renderingRateString = "Equals: ";
  QString num;
  num.setNum(1000.0/renderingInterval);
  renderingRateString = renderingRateString+num+" frames/second";
  mRenderingRateLabel = new QLabel(renderingRateString);
  
  mShadingCheckBox = new QCheckBox(tr("ShadingOn"));
  mShadingCheckBox->setChecked(shadingOn);
  
  //Layout
  mMainLayout = new QGridLayout;
  mMainLayout->addWidget(renderingIntervalLabel, 0, 0);
  mMainLayout->addWidget(mRenderingIntervalSpinBox, 0, 1);
  mMainLayout->addWidget(mRenderingRateLabel, 0, 2);
  mMainLayout->addWidget(mShadingCheckBox, 2, 0);
  setLayout(mMainLayout);
}

void PerformanceTab::renderingIntervalSlot(int interval)
{    
  QString renderingRateString = "Equals: ";
  QString num;
  //num.setNum(1000.0/mRenderingIntervalSpinBox->value());
  num.setNum(1000.0/interval);
  renderingRateString = renderingRateString+num+" frames/second";
  mRenderingRateLabel->setText(renderingRateString);
  mMainLayout->addWidget(mRenderingRateLabel, 0, 2);
} 

void PerformanceTab::saveParametersSlot()
{
  int renderingInterval = mSettings->value("renderingInterval").toInt();
  bool shadingOn = mSettings->value("shadingOn").toBool();
  
  if(renderingInterval != mRenderingIntervalSpinBox->value())
  {
    mSettings->setValue("renderingInterval", mRenderingIntervalSpinBox->value());
    emit renderingIntervalChanged(mRenderingIntervalSpinBox->value());
  }
  if(shadingOn != mShadingCheckBox->isChecked())
  {
    mSettings->setValue("shadingOn", mShadingCheckBox->isChecked());
    emit shadingChanged(mShadingCheckBox->isChecked());
  }
}

PreferencesDialog::PreferencesDialog(QWidget *parent) :
  QDialog(parent),
  mViewManager(ViewManager::getInstance())
{
  mFoldersTab = new FoldersTab;
  mFoldersTab->init();
  mPerformanceTab = new PerformanceTab;
  mPerformanceTab->init();
  
  connect(mPerformanceTab,
          SIGNAL(renderingIntervalChanged(int)),
          mViewManager,
          SLOT(renderingIntervalChangedSlot(int)));
  connect(mPerformanceTab,
          SIGNAL(shadingChanged(bool)),
          mViewManager,
          SLOT(shadingChangedSlot(bool)));
  
  tabWidget = new QTabWidget;
	tabWidget->addTab(mFoldersTab, tr("Folders"));
  tabWidget->addTab(mPerformanceTab, tr("Performance"));
	
	buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
  connect(buttonBox, SIGNAL(accepted()), mFoldersTab, SLOT(saveParametersSlot()));
  connect(buttonBox, SIGNAL(accepted()), mPerformanceTab, SLOT(saveParametersSlot()));
  connect(mFoldersTab, SIGNAL(savedParameters()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
  
  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addWidget(tabWidget);
  mainLayout->addWidget(buttonBox);
  setLayout(mainLayout);
	resize(700, 300);

}

PreferencesDialog::~PreferencesDialog()
{
}

}//namespace cx
