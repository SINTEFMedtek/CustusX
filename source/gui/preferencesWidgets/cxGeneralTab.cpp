#include "cxGeneralTab.h"

#include <QLabel>
#include <QToolButton>
#include <QGridLayout>
#include <QFileDialog>
#include "cxSettings.h"
#include "cxStateService.h"
#include "cxApplicationStateMachine.h"
#include "cxVLCRecorder.h"
#include "cxDataAdapterHelper.h"

namespace cx
{

GeneralTab::GeneralTab(QWidget *parent) :
		PreferenceTab(parent), mVLCPath("")
{
	mPatientDataFolderComboBox = NULL;
	mVLCPathComboBox = NULL;
	mToolConfigFolderComboBox = NULL;
	mChooseApplicationComboBox = NULL;
}

void GeneralTab::init()
{
  mGlobalPatientDataFolder = settings()->value("globalPatientDataFolder").toString();
  mVLCPath = settings()->value("vlcPath").toString();
  if(!QFile::exists(mVLCPath))
	  this->searchForVLC();

  connect(stateService()->getApplication().get(), SIGNAL(activeStateChanged()), this, SLOT(applicationStateChangedSlot()));

  // patientDataFolder
  QLabel* patientDataFolderLabel = new QLabel(tr("Patient data folder:"));
  mPatientDataFolderComboBox = new QComboBox;
  mPatientDataFolderComboBox->addItem( mGlobalPatientDataFolder);
  QAction* browsePatientFolderAction = new QAction(QIcon(":/icons/open.png"), tr("B&rowse..."), this);
  connect(browsePatientFolderAction, SIGNAL(triggered()), this, SLOT(browsePatientDataFolderSlot()));
  QToolButton* browsePatientFolderButton = new QToolButton(this);
  browsePatientFolderButton->setDefaultAction(browsePatientFolderAction);

  // VLC
  QLabel* vlcPathLabel = new QLabel(tr("VLC path:"));
  mVLCPathComboBox = new QComboBox();
  mVLCPathComboBox->addItem( mVLCPath);
  QAction* browseVLCPathAction = new QAction(QIcon(":/icons/open.png"), tr("Browse for VLC..."), this);
  connect(browseVLCPathAction, SIGNAL(triggered()), this, SLOT(browseVLCPathSlot()));
  QToolButton* browseVLCPathButton = new QToolButton(this);
  browseVLCPathButton->setDefaultAction(browseVLCPathAction);

  // Choose application name
  QLabel* chooseApplicationLabel = new QLabel(tr("Choose application:"));
  mChooseApplicationComboBox = new QComboBox();
  setApplicationComboBox();
  connect(mChooseApplicationComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(currentApplicationChangedSlot(int)));
  this->applicationStateChangedSlot();

  bool filterToolPositions = settings()->value("TrackingPositionFilter/enabled").value<bool>();
  mFilterToolPositions = BoolDataAdapterXml::initialize("Tool smoothing", "",
												 "Smooth the tool tracking positions using a low-pass filter.",
												 filterToolPositions);

  // Layout
  QGridLayout *mainLayout = new QGridLayout;
  mainLayout->addWidget(patientDataFolderLabel, 0, 0);
  mainLayout->addWidget(mPatientDataFolderComboBox, 0, 1);
  mainLayout->addWidget(browsePatientFolderButton, 0, 2);

  mainLayout->addWidget(chooseApplicationLabel, 1, 0);
  mainLayout->addWidget(mChooseApplicationComboBox, 1, 1);

  mainLayout->addWidget(vlcPathLabel, 2, 0);
  mainLayout->addWidget(mVLCPathComboBox, 2, 1);
  mainLayout->addWidget(browseVLCPathButton, 2, 2);

  createDataWidget(this, mFilterToolPositions, mainLayout, 3);
//  mainLayout->addWidget(createDataWidget(this, mFilterToolPositions ));

  mTopLayout->addLayout(mainLayout);
}

GeneralTab::~GeneralTab()
{}

void GeneralTab::browsePatientDataFolderSlot()
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

void GeneralTab::browseVLCPathSlot()
{
	mVLCPath = QFileDialog::getOpenFileName(this, tr("Find VLC executable"));

	if(!mVLCPath.isEmpty())
	{
		mVLCPathComboBox->addItem( mVLCPath );
		mVLCPathComboBox->setCurrentIndex( mVLCPathComboBox->currentIndex() + 1 );
	}
}

void GeneralTab::setApplicationComboBox()
{
  mChooseApplicationComboBox->blockSignals(true);
  mChooseApplicationComboBox->clear();
  QList<QAction*> actions = stateService()->getApplication()->getActionGroup()->actions();
  for (int i=0; i<actions.size(); ++i)
  {
    mChooseApplicationComboBox->insertItem(i, QIcon(), actions[i]->text(), actions[i]->data());
    if (actions[i]->isChecked())
      mChooseApplicationComboBox->setCurrentIndex(i);
  }

  mChooseApplicationComboBox->blockSignals(false);
}

void GeneralTab::searchForVLC(QStringList searchPaths)
{
	vlc()->findVLCApplication(searchPaths);
	if(vlc()->hasVLCApplication())
	  mVLCPath = vlc()->getVLCPath();
}

void GeneralTab::applicationStateChangedSlot()
{
  mChooseApplicationComboBox->blockSignals(true);
  QList<QAction*> actions = stateService()->getApplication()->getActionGroup()->actions();
  for (int i=0; i<actions.size(); ++i)
  {
    if (actions[i]->isChecked())
      mChooseApplicationComboBox->setCurrentIndex(i);
  }

  mChooseApplicationComboBox->blockSignals(false);
}

void GeneralTab::currentApplicationChangedSlot(int index)
{
  QList<QAction*> actions = stateService()->getApplication()->getActionGroup()->actions();
  if (index<0 || index>=actions.size())
    return;
  actions[index]->trigger();
}

void GeneralTab::saveParametersSlot()
{
  settings()->setValue("globalPatientDataFolder", mGlobalPatientDataFolder);
  settings()->setValue("vlcPath", mVLCPath);
  settings()->setValue("TrackingPositionFilter/enabled", mFilterToolPositions->getValue());
  settings()->sync();

  emit savedParameters();
}

} /* namespace cx */
