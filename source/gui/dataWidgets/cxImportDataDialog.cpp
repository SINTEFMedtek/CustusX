#include "cxImportDataDialog.h"

#include <cmath>
#include <QFileDialog>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <Qt>
#include <QCheckBox>
#include <QTimer>
#include <vtkImageData.h>
#include "sscLabeledComboBoxWidget.h"
#include "sscMessageManager.h"
#include "sscTypeConversions.h"
#include "sscData.h"
#include "sscDataManager.h"
#include "sscRegistrationTransform.h"
#include "sscImageAlgorithms.h"
#include "sscImage.h"
#include "cxStateService.h"
#include "cxPatientData.h"
#include "cxPatientService.h"
#include "cxViewManager.h"
#include "sscVolumeHelpers.h"

namespace cx
{

ImportDataDialog::ImportDataDialog(QString filename, QWidget* parent) :
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

  mModalityAdapter = DataModalityStringDataAdapter::New();
  mModalityCombo = new ssc::LabeledComboBoxWidget(this, mModalityAdapter);
  layout->addWidget(mModalityCombo);

  mImageTypeAdapter = ImageTypeStringDataAdapter::New();
  mImageTypeCombo = new ssc::LabeledComboBoxWidget(this, mImageTypeAdapter);
  layout->addWidget(mImageTypeCombo);

  mParentFrameAdapter = SetParentFrameStringDataAdapter::New();
  mParentFrameCombo = new ssc::LabeledComboBoxWidget(this, mParentFrameAdapter);
  layout->addWidget(mParentFrameCombo);

  mNiftiFormatCheckBox = new QCheckBox("Use NIfTI-1/ITK-Snap axis definition", this);
  mNiftiFormatCheckBox->setToolTip(""
	  "Use X=Left->Right Y=Posterior->Anterior Z=Inferior->Superior, as in ITK-Snap.\n"
	  "This is different from normal DICOM.");
  mNiftiFormatCheckBox->setChecked(false);
  mNiftiFormatCheckBox->setEnabled(false);
  mTransformFromParentFrameCheckBox = new QCheckBox("Import transform from Parent", this);
  mTransformFromParentFrameCheckBox->setToolTip("Replace data transform with that of the parent data.");
  mTransformFromParentFrameCheckBox->setChecked(false);

  mConvertToUnsignedCheckBox = new QCheckBox("Convert to unsigned", this);
  mConvertToUnsignedCheckBox->setToolTip(""
	  "Convert imported data set to unsigned values.\n"
	  "This is recommended on Linux because the 2D overlay\n"
	  "renderer only handles unsigned.");
  mConvertToUnsignedCheckBox->setChecked(false);

  layout->addWidget(mNiftiFormatCheckBox);
  layout->addWidget(mTransformFromParentFrameCheckBox);
  layout->addWidget(mConvertToUnsignedCheckBox);

  connect(mParentFrameAdapter.get(), SIGNAL(changed()), this, SLOT(updateImportTransformButton()));
  this->updateImportTransformButton();

  mErrorLabel = new QLabel();
  layout->addWidget(mErrorLabel);

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

ImportDataDialog::~ImportDataDialog()
{
}

void ImportDataDialog::showEvent(QShowEvent* event)
{
  // the import operation takes up to a few seconds. Call it AFTER the dialog is up and running its own message loop,
  // this avoids all problems related to modal vs right-click in the main window.
  QTimer::singleShot(0, this, SLOT(importDataSlot()));
}

void ImportDataDialog::importDataSlot()
{
  QString infoText;
  mData = patientService()->getPatientData()->importData(mFilename, infoText);
  if (!infoText.isEmpty())
  {
	  infoText += "<font color=red><br>If these warnings are not expected the import have probably failed.</font>";
	  if(infoText.contains("File already exists", Qt::CaseInsensitive))
		  infoText += "<font color=red><br>Importing two different volumes with the same name will lead to undesired effects.</font>";
	  mErrorLabel->setText(infoText);
  }

  if (!mData)
  {
    mUidLabel->setText(mFilename);
    mNameLabel->setText("Import failed");
    mOkButton->setText("Exit");
    return;
  }

  mUidLabel->setText("Data uid:  " + qstring_cast(mData->getUid()));
  mNameLabel->setText("Data name: " + qstring_cast(mData->getName()));

  ssc::ImagePtr image = boost::shared_dynamic_cast<ssc::Image>(mData);
  mModalityAdapter->setData(image);
  mModalityCombo->setEnabled(image!=0);
  mImageTypeAdapter->setData(image);
  mImageTypeCombo->setEnabled(image!=0);

  this->setInitialGuessForParentFrame();
  mParentFrameAdapter->setData(mData);
  mParentFrameCombo->setEnabled(ssc::dataManager()->getData().size()>1);

  // enable nifti imiport only for meshes. (as this is the only case we have seen)
  mNiftiFormatCheckBox->setEnabled(ssc::dataManager()->getMesh(mData->getUid())!=0);

  mConvertToUnsignedCheckBox->setEnabled(false);
//  ssc::ImagePtr image = boost::shared_dynamic_cast<ssc::Image>(mData);
  if (image && image->getBaseVtkImageData())
  {
	  vtkImageDataPtr img = image->getBaseVtkImageData();
//	  std::cout << "type " << img->GetScalarTypeAsString() << " -- " << img->GetScalarType() << std::endl;
//	  std::cout << "range " << img->GetScalarTypeMin() << " -- " << img->GetScalarTypeMax() << std::endl;
	  mConvertToUnsignedCheckBox->setEnabled( (image!=0) && (image->getBaseVtkImageData()->GetScalarTypeMin()<0) );
#ifndef __APPLE__
#ifndef WIN32
	  // i.e. LINUX:
	  if (mConvertToUnsignedCheckBox->isEnabled())
		  mConvertToUnsignedCheckBox->setChecked(true);
#endif // WIN32
#endif // __APPLE__
  }
}


/** Use heuristics to guess a parent frame,
 *  based on similarities in name.
 */
void ImportDataDialog::setInitialGuessForParentFrame()
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
      mData->get_rMd_History()->setParentSpace(iter->first);
      break;
    }
  }

}

void ImportDataDialog::updateImportTransformButton()
{
  ssc::DataPtr parent = ssc::dataManager()->getData(mParentFrameAdapter->getValue());
  bool enabled = bool(parent);
  mTransformFromParentFrameCheckBox->setEnabled(enabled);
}

void ImportDataDialog::acceptedSlot()
{
  this->importParentTransform();
  this->convertFromNifti1Coordinates();
  this->convertToUnsigned();

	patientService()->getPatientData()->autoSave();
	viewManager()->autoShowData(mData);
}

/** According to
 * http://www.itksnap.org/pmwiki/pmwiki.php?n=Documentation.ReleaseNotesVersion20 (search NIFTI)
 * http://niftilib.sourceforge.net/ (FAQ item 14)
 *
 * ITK-Snap uses NIfTI coordinates, which have reversed signs on the x and y axis, relative to the DICOM spec.
 * Solve by rotating Z 180 deg.
 */
void ImportDataDialog::convertFromNifti1Coordinates()
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
void ImportDataDialog::importParentTransform()
{
  if (!mTransformFromParentFrameCheckBox->isChecked())
    return;
  if(!mData)
    return;
  ssc::DataPtr parent = ssc::dataManager()->getData(mData->getParentSpace());
  if (!parent)
    return;
  mData->get_rMd_History()->setRegistration(parent->get_rMd());
  ssc::messageManager()->sendInfo("Assigned rMd from data [" + parent->getName() + "] to data [" + mData->getName() + "]");
}

void ImportDataDialog::convertToUnsigned()
{
	if (!mConvertToUnsignedCheckBox->isChecked())
		return;

	ssc::ImagePtr image = boost::shared_dynamic_cast<ssc::Image>(mData);
	if (!image)
		return;

//		vtkImageDataPtr img0 = image->getBaseVtkImageData();
//		std::cout << "type " << img0->GetScalarTypeAsString() << " -- " << img0->GetScalarType() << std::endl;
//		std::cout << "range " << img0->GetScalarTypeMin() << " -- " << img0->GetScalarTypeMax() << std::endl;

	vtkImageDataPtr img = ssc::convertImageToUnsigned(image)->getBaseVtkImageData();

	image->setVtkImageData(img);
	ssc::dataManager()->saveImage(image, patientService()->getPatientData()->getActivePatientFolder());
}


}//namespace cx
