#include "cxGeneralTab.h"

#include <QLabel>
#include <QToolButton>
#include <QGridLayout>
#include <QFileDialog>
#include "cxSettings.h"
#include "cxStateService.h"
#include "cxApplicationStateMachine.h"

namespace cx
{

GeneralTab::GeneralTab(QWidget *parent) :
		PreferenceTab(parent)
{
	mPatientDataFolderComboBox = NULL;
	mToolConfigFolderComboBox = NULL;
	mChooseApplicationComboBox = NULL;
}

void GeneralTab::init()
{
  mGlobalPatientDataFolder = settings()->value("globalPatientDataFolder").toString();

  connect(stateService()->getApplication().get(), SIGNAL(activeStateChanged()), this, SLOT(applicationStateChangedSlot()));

  // patientDataFolder
  QLabel* patientDataFolderLabel = new QLabel(tr("Patient data folder:"));

  mPatientDataFolderComboBox = new QComboBox;
  mPatientDataFolderComboBox->addItem( mGlobalPatientDataFolder);

  QAction* browsePatientFolderAction = new QAction(QIcon(":/icons/open.png"), tr("B&rowse..."), this);
  connect(browsePatientFolderAction, SIGNAL(triggered()), this, SLOT(browsePatientDataFolderSlot()));
  QToolButton* browsePatientFolderButton = new QToolButton(this);
  browsePatientFolderButton->setDefaultAction(browsePatientFolderAction);

  // Choose application name
  QLabel* chooseApplicationLabel = new QLabel(tr("Choose application:"));
  mChooseApplicationComboBox = new QComboBox();
  setApplicationComboBox();
  connect(mChooseApplicationComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(currentApplicationChangedSlot(int)));
  this->applicationStateChangedSlot();

  // Layout
  QGridLayout *mainLayout = new QGridLayout;
  mainLayout->addWidget(patientDataFolderLabel, 0, 0);
  mainLayout->addWidget(mPatientDataFolderComboBox, 0, 1);
  mainLayout->addWidget(browsePatientFolderButton, 0, 2);

  mainLayout->addWidget(chooseApplicationLabel, 8, 0);
  mainLayout->addWidget(mChooseApplicationComboBox, 8, 1);

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
  settings()->sync();

  emit savedParameters();
}

} /* namespace cx */
