#include "cxImportDataWizard.h"

#include <cmath>
#include <QFileDialog>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <Qt>
#include <QCheckBox>
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

  mParentFrameAdapter = SetParentFrameStringDataAdapter::New();
  mParentFrameCombo = new ssc::LabeledComboBoxWidget(this, mParentFrameAdapter);
  layout->addWidget(mParentFrameCombo);

  mNiftiFormatCheckBox = new QCheckBox("Use NIfTI-1/ITK-Snap axis definition", this);
  mNiftiFormatCheckBox->setToolTip("Use X=Left->Right Y=Posterior->Anterior Z=Inferior->Superior, as in ITK-Snap.");
  mNiftiFormatCheckBox->setChecked(false);
  mNiftiFormatCheckBox->setEnabled(false);
  mTransformFromParentFrameCheckBox = new QCheckBox("Import transform from Parent", this);
  mTransformFromParentFrameCheckBox->setToolTip("Replace data transform with that of the parent data.");
  mTransformFromParentFrameCheckBox->setChecked(false);
  layout->addWidget(mNiftiFormatCheckBox);
  layout->addWidget(mTransformFromParentFrameCheckBox);

  connect(mParentFrameAdapter.get(), SIGNAL(changed()), this, SLOT(updateImportTransformButton()));
  this->updateImportTransformButton();

  QHBoxLayout* buttons = new QHBoxLayout;
  layout->addLayout(buttons);
  mOkButton = new QPushButton("OK", this);
  buttons->addStretch();
  buttons->addWidget(mOkButton);
  connect(mOkButton, SIGNAL(clicked()), this, SLOT(accept()));
  connect(this, SIGNAL(accepted()), this, SLOT(acceptedSlot()));
  mOkButton->setDefault(true);
  mOkButton->setFocus();

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
  {
    mUidLabel->setText(mFilename);
    mNameLabel->setText("Import failed");
    mOkButton->setText("Exit");
    return;
  }

  mUidLabel->setText("Data uid:  " + qstring_cast(mData->getUid()));
  mNameLabel->setText("Data name: " + qstring_cast(mData->getName()));

  this->setInitialGuessForParentFrame();
  mParentFrameAdapter->setData(mData);
  mParentFrameCombo->setEnabled(ssc::dataManager()->getData().size()>1);

  // enable nifti imiport only for meshes. (as this is the only case we have seen)
  mNiftiFormatCheckBox->setEnabled(ssc::dataManager()->getMesh(mData->getUid())!=0);

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
      mData->get_rMd_History()->setParentFrame(iter->first);
      break;
    }
  }

}

void ImportDataWizard::updateImportTransformButton()
{
  ssc::DataPtr parent = ssc::dataManager()->getData(mParentFrameAdapter->getValue());
  bool enabled = bool(parent);
  mTransformFromParentFrameCheckBox->setEnabled(enabled);
}

void ImportDataWizard::acceptedSlot()
{
  this->importParentTransform();
  this->convertFromNifti1Coordinates();
}

/** According to
 * http://www.itksnap.org/pmwiki/pmwiki.php?n=Documentation.ReleaseNotesVersion20 (search NIFTI)
 * http://niftilib.sourceforge.net/ (FAQ item 14)
 *
 * ITK-Snap uses NIfTI coordinates, which have reversed signs on the x and y axis, relative to the DICOM spec.
 * Solve by rotating Z 180 deg.
 */
void ImportDataWizard::convertFromNifti1Coordinates()
{
  if (!mNiftiFormatCheckBox->isChecked())
    return;
  if(!mData)
    return;
  ssc::Transform3D rMd = mData->get_rMd();
  rMd = rMd * ssc::createTransformRotateZ(M_PI);
  mData->get_rMd_History()->setRegistration(rMd);
  ssc::messageManager()->sendInfo("Nifti import: rotated input data " + mData->getName() + " 180* around Z-axis.");
}

/** Apply the transform from the parent frame to the imported data.
 *
 */
void ImportDataWizard::importParentTransform()
{
  if (!mTransformFromParentFrameCheckBox->isChecked())
    return;
  if(!mData)
    return;
  ssc::DataPtr parent = ssc::dataManager()->getData(mData->getParentFrame());
  if (!parent)
    return;
  mData->get_rMd_History()->setRegistration(parent->get_rMd());
  ssc::messageManager()->sendInfo("Assigned rMd from data [" + parent->getName() + "] to data [" + mData->getName() + "]");
}

}//namespace cx
