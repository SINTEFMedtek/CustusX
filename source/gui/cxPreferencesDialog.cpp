
#include <QtGui>

#include "sscMessageManager.h"
#include "cxPreferencesDialog.h"
#include "cxViewManager.h"
#include "cxDataLocations.h"
#include <iostream>
#include "cxToolManager.h"
#include "cxDataLocations.h"
#include "cxStateMachineManager.h"
#include "sscDoubleWidgets.h"

namespace cx
{
FoldersTab::FoldersTab(QWidget *parent) :
  QWidget(parent),
  mSettings(DataLocations::getSettings())
{
}

void FoldersTab::init()
{
  mGlobalPatientDataFolder = mSettings->value("globalPatientDataFolder").toString();

  connect(stateManager()->getApplication().get(), SIGNAL(activeStateChanged()), this, SLOT(applicationStateChangedSlot()));

  // patientDataFolder
  QLabel* patientDataFolderLabel = new QLabel(tr("Patient data folder:"));

  mPatientDataFolderComboBox = new QComboBox;
  mPatientDataFolderComboBox->addItem( mGlobalPatientDataFolder);

  QPushButton* patientDataFolderButton = new QPushButton(tr("B&rowse..."));
  connect( patientDataFolderButton, 
          SIGNAL(clicked()), 
          this, 
          SLOT(browsePatientDataFolderSlot()) );
  
  QLabel *toolConfigFilesLabel = new QLabel(tr("Tool configuration files:"));
  mToolConfigFilesComboBox = new QComboBox;
  connect( mToolConfigFilesComboBox, 
          SIGNAL(currentIndexChanged(const QString &)),
          this, 
          SLOT(currentToolConfigFilesIndexChangedSlot(const QString &)) );
  
  // Choose application name
  QLabel* chooseApplicationLabel = new QLabel(tr("Choose application:"));
  mChooseApplicationComboBox = new QComboBox();
  setApplicationComboBox();
  connect(mChooseApplicationComboBox,
          SIGNAL(currentIndexChanged(int)),
          this,
          SLOT(currenApplicationChangedSlot(int)));
  mCurrentToolConfigFile = mSettings->value("toolConfigFile").toString();
  applicationStateChangedSlot();
  
  // Layout
  QGridLayout *mainLayout = new QGridLayout;
  mainLayout->addWidget(patientDataFolderLabel, 0, 0);
  mainLayout->addWidget(mPatientDataFolderComboBox, 0, 1);
  mainLayout->addWidget(patientDataFolderButton, 0, 2);
  
  mainLayout->addWidget(chooseApplicationLabel, 8, 0);
  mainLayout->addWidget(mChooseApplicationComboBox, 8, 1);
  
  mainLayout->addWidget(toolConfigFilesLabel, 9, 0);
  mainLayout->addWidget(mToolConfigFilesComboBox, 9, 1);
 
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

void FoldersTab::currentToolConfigFilesIndexChangedSlot(const QString & newToolConfigFile)
{
  mCurrentToolConfigFile = newToolConfigFile;
}

void FoldersTab::setToolConfigComboBox()
{
  mToolConfigFilesComboBox->blockSignals(true);
	QDir dir(DataLocations::getApplicationToolConfigPath());
    dir.setFilter(QDir::Files);

    QStringList nameFilters;
    nameFilters << "*.xml";
    dir.setNameFilters(nameFilters);

    QStringList list = dir.entryList();

    mToolConfigFilesComboBox->clear();
    mToolConfigFilesComboBox->addItems( list );

    int currentIndex = mToolConfigFilesComboBox->findText( mCurrentToolConfigFile );
    mToolConfigFilesComboBox->setCurrentIndex( currentIndex );
    mToolConfigFilesComboBox->blockSignals(false);
}

void FoldersTab::setApplicationComboBox()
{
  mChooseApplicationComboBox->blockSignals(true);
  mChooseApplicationComboBox->clear();
  QList<QAction*> actions = stateManager()->getApplication()->getActionGroup()->actions();
  for (int i=0; i<actions.size(); ++i)
  {
    mChooseApplicationComboBox->insertItem(i, QIcon(), actions[i]->text(), actions[i]->data());
    if (actions[i]->isChecked())
      mChooseApplicationComboBox->setCurrentIndex(i);
  }

  mChooseApplicationComboBox->blockSignals(false);
}

void FoldersTab::applicationStateChangedSlot()
{
  mChooseApplicationComboBox->blockSignals(true);
  QList<QAction*> actions = stateManager()->getApplication()->getActionGroup()->actions();
  for (int i=0; i<actions.size(); ++i)
  {
    if (actions[i]->isChecked())
      mChooseApplicationComboBox->setCurrentIndex(i);
  }

  mChooseApplicationComboBox->blockSignals(false);

  this->setToolConfigComboBox();

  // this hack ensures that the tool folder is reinitialized when changing application.
  // TODO: move to application state
  if (mToolConfigFilesComboBox->currentIndex()<0)
    mToolConfigFilesComboBox->setCurrentIndex(0);
}
  
void FoldersTab::currenApplicationChangedSlot(int index)
{
  QList<QAction*> actions = stateManager()->getApplication()->getActionGroup()->actions();
  if (index<0 || index>=actions.size())
    return;
  actions[index]->trigger();
}
  
void FoldersTab::saveParametersSlot()
{
  // currentPatientDataFolder
  mSettings->setValue("globalPatientDataFolder", mGlobalPatientDataFolder);
  
  // currentToolConfigFile
  mSettings->setValue("toolConfigFile", mCurrentToolConfigFile);
  
  mSettings->sync();

  // update toolmanager config file
  ToolManager::getInstance()->setConfigurationFile(DataLocations::getToolConfigFilePath());

  emit savedParameters();
}
  
//==============================================================================
// PerformanceTab
//------------------------------------------------------------------------------
PerformanceTab::PerformanceTab(QWidget *parent) :
  QWidget(parent),
  mSettings(DataLocations::getSettings())
{
}

void PerformanceTab::init()
{
  int renderingInterval = mSettings->value("renderingInterval").toInt();
  
  QLabel* renderingIntervalLabel = new QLabel(tr("Rendering interval"));
  
  mRenderingIntervalSpinBox = new QSpinBox;
  mRenderingIntervalSpinBox->setSuffix("ms");
  mRenderingIntervalSpinBox->setMinimum(16);
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
  
  double Mb = pow(10.0,6);
  bool ok = true;
  double maxRenderSize = mSettings->value("maxRenderSize").toDouble(&ok);
  if (!ok)
    maxRenderSize = 10 * Mb;
  mMaxRenderSize = ssc::DoubleDataAdapterXml::initialize("MaxRenderSize", "Max Render Size (Mb)", "Maximum size of volumes used in volume rendering. Applies to new volumes.", maxRenderSize, ssc::DoubleRange(1*Mb,300*Mb,1*Mb), 0, QDomNode());
  mMaxRenderSize->setInternal2Display(1.0/Mb);

  mSmartRenderCheckBox = new QCheckBox("Smart Render");
  mSmartRenderCheckBox->setChecked(viewManager()->getSmartRender());

  bool useGPURender = mSettings->value("useGPUVolumeRayCastMapper").toBool();
  mGPURenderCheckBox = new QCheckBox("Use GPU 3D Renderer");
  mGPURenderCheckBox->setChecked(useGPURender);

  //Layout
  mMainLayout = new QGridLayout;
  mMainLayout->addWidget(renderingIntervalLabel, 0, 0);
  new ssc::SliderGroupWidget(this, mMaxRenderSize, mMainLayout, 1);
  mMainLayout->addWidget(mRenderingIntervalSpinBox, 0, 1);
  mMainLayout->addWidget(mRenderingRateLabel, 0, 2);
  mMainLayout->addWidget(mSmartRenderCheckBox, 2, 0);
  mMainLayout->addWidget(mGPURenderCheckBox, 3, 0);

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
  
  if(renderingInterval != mRenderingIntervalSpinBox->value())
  {
    mSettings->setValue("renderingInterval", mRenderingIntervalSpinBox->value());
    emit renderingIntervalChanged(mRenderingIntervalSpinBox->value());
  }

  mSettings->setValue("useGPUVolumeRayCastMapper", mGPURenderCheckBox->isChecked());
  mSettings->setValue("maxRenderSize", mMaxRenderSize->getValue());

  viewManager()->setSmartRender(mSmartRenderCheckBox->isChecked());
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
{}

}//namespace cx
