
#include <QtGui>

#include "cxPreferencesDialog.h"

#include <iostream>

namespace cx
{
FoldersTab::FoldersTab(QWidget *parent) :
  QWidget(parent),
  mSettings(new QSettings())
{}

void FoldersTab::init(){
  mCurrentPatientDataFolder = mSettings->value("mainWindow/patientDataFolder").toString();
  //mCurrentImportDataFolder  = mSettings->value("mainWindow/importDataFolder").toString();
  mCurrentToolConfigFilePath = mSettings->value("toolManager/toolConfigFilePath").toString();

  QFileInfo fileInfo( mCurrentToolConfigFilePath );
  mCurrentToolConfigFolder = fileInfo.absolutePath();
  mCurrentToolConfigFile = fileInfo.fileName();

  // patientDataFolder
  QLabel* patientDataFolderLabel = new QLabel(tr("Patient data folder:"));

  mPatientDataFolderComboBox = new QComboBox;
  mPatientDataFolderComboBox->addItem( mCurrentPatientDataFolder);
  //patientDataFolderComboBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

  QPushButton* patientDataFolderButton = new QPushButton(tr("&Browse..."));
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
 
  setLayout(mainLayout);
  
}

FoldersTab::~FoldersTab()
{}

void FoldersTab::browsePatientDataFolderSlot()
{
  mCurrentPatientDataFolder = QFileDialog::getExistingDirectory(this, 
                                                     tr("Find Patient Data Folder"), 
                                                     mCurrentPatientDataFolder,
                                                     QFileDialog::ShowDirsOnly);
  if( !mCurrentPatientDataFolder.isEmpty() ) {
    mPatientDataFolderComboBox->addItem( mCurrentPatientDataFolder );
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

void FoldersTab::setCurrentToolConfigFile()
{
  int currentIndex = mToolConfigFilesComboBox->findText( mCurrentToolConfigFile );
  mToolConfigFilesComboBox->setCurrentIndex( currentIndex );
}
  
void FoldersTab::saveParametersSlot()
{
  
  // currentPatientDataFolder
  mSettings->setValue("mainWindow/patientDataFolder", mCurrentPatientDataFolder);
  //mSettings->setValue("mainWindow/importDataFolder",  mCurrentImportDataFolder);
  
  // currentToolConfigFilePath
  QDir dir(mCurrentToolConfigFolder);
  mCurrentToolConfigFilePath = dir.absoluteFilePath( mCurrentToolConfigFile );
  mSettings->setValue("toolManager/toolConfigFilePath", mCurrentToolConfigFilePath);
  
  mSettings->sync();
  emit savedParameters();
}
  
//==============================================================================
// TestTab
//------------------------------------------------------------------------------
TestTab::TestTab(QWidget *parent)
    : QWidget(parent)
{
}

TestTab::~TestTab()
{
}

PreferencesDialog::PreferencesDialog(QWidget *parent)
    : QDialog(parent)
{
  mFoldersTab = new FoldersTab;
  mFoldersTab->init();
  //testTab = new TestTab;
  
  tabWidget = new QTabWidget;
	tabWidget->addTab(mFoldersTab, tr("Folders"));
  //tabWidget->addTab(testTab, tr("Test"));
	
	buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
  connect(buttonBox, SIGNAL(accepted()), mFoldersTab, SLOT(saveParametersSlot()));
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
