#include "cxImportDataWizard.h"

#include <QFileDialog>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <Qt>
#include <QTimer>
#include "sscLabeledComboBoxWidget.h"
#include "sscMessageManager.h"
#include "sscTypeConversions.h"
#include "sscData.h"
#include "sscDataManager.h"
#include "sscRegistrationTransform.h"
#include "cxStateMachineManager.h"
#include "cxPatientData.h"

namespace cx
{


//ImportDataWizard::ImportDataWizard(ssc::DataPtr data, QWidget* parent) :
//    QDialog(parent),
//    mData(data)
//{
//  this->setAttribute(Qt::WA_DeleteOnClose);
//
//  QVBoxLayout* layout = new QVBoxLayout(this);
//  this->setWindowTitle("Set properties for imported data");
//
//  layout->addWidget(new QLabel("Data uid:  " + qstring_cast(data->getUid())));
//  layout->addWidget(new QLabel("Data name: " + qstring_cast(data->getName())));
//
//  this->setInitialGuessForParentFrame();
//
//  mParentFrameAdapter = ParentFrameStringDataAdapter::New();
//  mParentFrameAdapter->setData(data);
//  ssc::LabeledComboBoxWidget*  combo = new ssc::LabeledComboBoxWidget(this, mParentFrameAdapter);
//  combo->setEnabled(ssc::dataManager()->getData().size()>1);
//  layout->addWidget(combo);
//
//  mImportTransformButton = new QPushButton("Import Transform from Parent", this);
//  mImportTransformButton->setToolTip("Replace data transform with that of the parent data.");
//  connect(mParentFrameAdapter.get(), SIGNAL(changed()), this, SLOT(updateImportTransformButton()));
//  connect(mImportTransformButton, SIGNAL(clicked()), this, SLOT(importTransformSlot()));
//  layout->addWidget(mImportTransformButton);
//  this->updateImportTransformButton();
//
//  QHBoxLayout* buttons = new QHBoxLayout;
//  layout->addLayout(buttons);
//  QPushButton* okButton = new QPushButton("OK", this);
//  buttons->addStretch();
//  buttons->addWidget(okButton);
//  connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));
//  okButton->setDefault(true);
//
//  ssc::messageManager()->sendInfo("Importing data...");
//}

ImportDataWizard::ImportDataWizard(QString filename, QWidget* parent) :
    QDialog(parent),
    mFilename(filename)
{
  this->setAttribute(Qt::WA_DeleteOnClose);

  QVBoxLayout* layout = new QVBoxLayout(this);
  this->setWindowTitle("Set properties for imported data");

  mUidLabel = new QLabel("Data uid:  ");
  mNameLabel = new QLabel("Data name: ");

  layout->addWidget(mUidLabel);
  layout->addWidget(mNameLabel);
//
//  this->setInitialGuessForParentFrame();

  mParentFrameAdapter = ParentFrameStringDataAdapter::New();
//  mParentFrameAdapter->setData(data);
  mParentFrameCombo = new ssc::LabeledComboBoxWidget(this, mParentFrameAdapter);
  //combo->setEnabled(ssc::dataManager()->getData().size()>1);
  layout->addWidget(mParentFrameCombo);

  mImportTransformButton = new QPushButton("Import Transform from Parent", this);
  mImportTransformButton->setToolTip("Replace data transform with that of the parent data.");
  connect(mParentFrameAdapter.get(), SIGNAL(changed()), this, SLOT(updateImportTransformButton()));
  connect(mImportTransformButton, SIGNAL(clicked()), this, SLOT(importTransformSlot()));
  layout->addWidget(mImportTransformButton);
  this->updateImportTransformButton();

  QHBoxLayout* buttons = new QHBoxLayout;
  layout->addLayout(buttons);
  QPushButton* okButton = new QPushButton("OK", this);
  buttons->addStretch();
  buttons->addWidget(okButton);
  connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));
  okButton->setDefault(true);

  ssc::messageManager()->sendInfo("Importing data...");
}

ImportDataWizard::~ImportDataWizard()
{
}

void ImportDataWizard::showEvent(QShowEvent* event)
{
  // the import operation takes up to a few seconds. Call it AFTER the dialog is up and running its own message loop,
  // this avoids all problems related to modal vs right-click in the main window.
  QTimer::singleShot(0, this, SLOT(importDataSlot()));
}

void ImportDataWizard::importDataSlot()
{
  mData = stateManager()->getPatientData()->importData(mFilename);

  if (!mData)
    return;

  mUidLabel->setText("Data uid:  " + qstring_cast(mData->getUid()));
  mNameLabel->setText("Data name: " + qstring_cast(mData->getName()));

  this->setInitialGuessForParentFrame();
  mParentFrameAdapter->setData(mData);
  mParentFrameCombo->setEnabled(ssc::dataManager()->getData().size()>1);
}


/** Use heuristics to guess a parent frame,
 *  based on similarities in name.
 */
void ImportDataWizard::setInitialGuessForParentFrame()
{
  if(!mData)
    return;

  QString base = qstring_cast(mData->getName()).split(".")[0];

  std::map<QString, ssc::DataPtr> all = ssc::dataManager()->getData();
  for (std::map<QString, ssc::DataPtr>::iterator iter=all.begin(); iter!=all.end(); ++iter)
  {
    if (iter->second==mData)
      continue;
    QString current = qstring_cast(iter->second->getName()).split(".")[0];
    if (base.indexOf(current)>=0)
    {
      mData->get_rMd_History()->addParentFrame(iter->first);
      break;
    }
  }

}

void ImportDataWizard::updateImportTransformButton()
{
  ssc::DataPtr parent = ssc::dataManager()->getData(mParentFrameAdapter->getValue());
  bool enabled = bool(parent);

  mImportTransformButton->setEnabled(enabled);
}

void ImportDataWizard::importTransformSlot()
{
  if(!mData)
    return;
  ssc::DataPtr parent = ssc::dataManager()->getData(mData->getParentFrame());
  if (!parent)
    return;
  mData->get_rMd_History()->setRegistration(parent->get_rMd());
  ssc::messageManager()->sendInfo("Assigned rMd from data [" + parent->getName() + "] to data [" + mData->getName() + "]");
}

}//namespace cx
