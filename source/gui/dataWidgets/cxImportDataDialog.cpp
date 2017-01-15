/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
=========================================================================*/

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
#include "cxLabeledComboBoxWidget.h"
#include "cxLogger.h"
#include "cxTypeConversions.h"
#include "cxData.h"
#include "cxRegistrationTransform.h"
#include "cxImageAlgorithms.h"
#include "cxImage.h"
#include "cxVolumeHelpers.h"
#include "cxImageTF3D.h"
#include "cxImageLUT2D.h"
#include "cxPatientModelService.h"
#include "cxMesh.h"
#include "cxViewService.h"


//TODO: remove
#include "cxLegacySingletons.h"

namespace cx
{

ImportDataDialog::ImportDataDialog(PatientModelServicePtr patientModelService, QString filename, QWidget* parent) :
    QDialog(parent),
	mFilename(filename),
	mPatientModelService(patientModelService)
{
  this->setAttribute(Qt::WA_DeleteOnClose);

  QVBoxLayout* layout = new QVBoxLayout(this);
  this->setWindowTitle("Set properties for imported data");

  mUidLabel = new QLabel("Data uid:  ");
  mNameLabel = new QLabel("Data name: ");

  layout->addWidget(mUidLabel);
  layout->addWidget(mNameLabel);

  mModalityAdapter = StringPropertyDataModality::New(mPatientModelService);
  mModalityCombo = new LabeledComboBoxWidget(this, mModalityAdapter);
  layout->addWidget(mModalityCombo);

  mImageTypeAdapter = StringPropertyImageType::New(mPatientModelService);
  mImageTypeCombo = new LabeledComboBoxWidget(this, mImageTypeAdapter);
  layout->addWidget(mImageTypeCombo);

  mParentFrameAdapter = StringPropertySetParentFrame::New(mPatientModelService);
  mParentFrameCombo = new LabeledComboBoxWidget(this, mParentFrameAdapter);
  layout->addWidget(mParentFrameCombo);

  mNiftiFormatCheckBox = new QCheckBox("Convert from RAS to LPS coordinates", this);
  mNiftiFormatCheckBox->setToolTip(""
	  "Use RAS (X=Left->Right Y=Posterior->Anterior Z=Inferior->Superior), as in ITK-Snap.\n"
	  "This is different from DICOM/CustusX, which uses LPS (left-posterior-superior).");
  mNiftiFormatCheckBox->setChecked(false);
  mNiftiFormatCheckBox->setEnabled(false);
  mTransformFromParentFrameCheckBox = new QCheckBox("Import transform from Parent", this);
  mTransformFromParentFrameCheckBox->setToolTip("Replace data transform with that of the parent data.");
  mTransformFromParentFrameCheckBox->setChecked(false);

  mConvertToUnsignedCheckBox = new QCheckBox("Convert to unsigned", this);
  mConvertToUnsignedCheckBox->setToolTip("Convert imported data set to unsigned values.");
  mConvertToUnsignedCheckBox->setChecked(false);

  layout->addWidget(mNiftiFormatCheckBox);
  layout->addWidget(mTransformFromParentFrameCheckBox);
  layout->addWidget(mConvertToUnsignedCheckBox);

  connect(mParentFrameAdapter.get(), &Property::changed, this, &ImportDataDialog::updateImportTransformButton);
  this->updateImportTransformButton();

  mErrorLabel = new QLabel();
  layout->addWidget(mErrorLabel);

  QHBoxLayout* buttons = new QHBoxLayout;
  layout->addLayout(buttons);
  mOkButton = new QPushButton("OK", this);
  buttons->addStretch();
  buttons->addWidget(mOkButton);
  connect(mOkButton, &QPushButton::clicked, this, &QDialog::accept);
  connect(this, &QDialog::accepted, this, &ImportDataDialog::acceptedSlot);
  connect(this, &QDialog::rejected, this, &ImportDataDialog::finishedSlot);
  mOkButton->setDefault(true);
  mOkButton->setFocus();

  report("Importing data...");
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
  mData = mPatientModelService->importData(mFilename, infoText);
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

  ImagePtr image = boost::dynamic_pointer_cast<Image>(mData);
  mModalityAdapter->setData(image);
  mModalityCombo->setEnabled(image!=0);
  mImageTypeAdapter->setData(image);
  mImageTypeCombo->setEnabled(image!=0);

  this->setInitialGuessForParentFrame();
  mParentFrameAdapter->setData(mData);
  mParentFrameCombo->setEnabled(mPatientModelService->getDatas().size()>1);

  // enable nifti imiport only for meshes. (as this is the only case we have seen)
  mNiftiFormatCheckBox->setEnabled(mPatientModelService->getData<Mesh>(mData->getUid())!=0);

  mConvertToUnsignedCheckBox->setEnabled(false);
  if (image && image->getBaseVtkImageData())
  {
//	  vtkImageDataPtr img = image->getBaseVtkImageData();
//	  std::cout << "type " << img->GetScalarTypeAsString() << " -- " << img->GetScalarType() << std::endl;
//	  std::cout << "range " << img->GetScalarTypeMin() << " -- " << img->GetScalarTypeMax() << std::endl;
	  mConvertToUnsignedCheckBox->setEnabled( (image!=0) && (image->getBaseVtkImageData()->GetScalarTypeMin()<0) );
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

  std::map<QString, DataPtr> all = mPatientModelService->getDatas();
  for (std::map<QString, DataPtr>::iterator iter=all.begin(); iter!=all.end(); ++iter)
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
  DataPtr parent = mPatientModelService->getData(mParentFrameAdapter->getValue());
  bool enabled = bool(parent);
  mTransformFromParentFrameCheckBox->setEnabled(enabled);
}

void ImportDataDialog::acceptedSlot()
{
	this->importParentTransform();
	this->convertFromNifti1Coordinates();
	this->convertToUnsigned();

	mPatientModelService->autoSave();
	viewService()->autoShowData(mData);
	this->finishedSlot();
}

void ImportDataDialog::finishedSlot()
{
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
  Transform3D sMd = mData->get_rMd();
  Transform3D sMr = createTransformFromReferenceToExternal(pcsRAS);
//  rMd = createTransformRotateZ(M_PI) * rMd;
  Transform3D rMd = sMr.inv() * sMd;
  mData->get_rMd_History()->setRegistration(rMd);
  report("Nifti import: Converted data " + mData->getName() + " from LPS to RAS coordinates.");
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
  DataPtr parent = mPatientModelService->getData(mData->getParentSpace());
  if (!parent)
    return;
  mData->get_rMd_History()->setRegistration(parent->get_rMd());
  report("Assigned rMd from data [" + parent->getName() + "] to data [" + mData->getName() + "]");
}

void ImportDataDialog::convertToUnsigned()
{
	if (!mConvertToUnsignedCheckBox->isChecked())
		return;

	ImagePtr image = boost::dynamic_pointer_cast<Image>(mData);
	if (!image)
		return;

	ImagePtr converted = convertImageToUnsigned(patientService(), image);

	image->setVtkImageData(converted->getBaseVtkImageData());

	ImageTF3DPtr TF3D = converted->getTransferFunctions3D()->createCopy();
	ImageLUT2DPtr LUT2D = converted->getLookupTable2D()->createCopy();
	image->setLookupTable2D(LUT2D);
	image->setTransferFunctions3D(TF3D);
	patientService()->insertData(image);
}


}//namespace cx
