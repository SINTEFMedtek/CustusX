/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxExportDataDialog.h"

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

#include "cxTypeConversions.h"
#include "cxData.h"
#include "cxRegistrationTransform.h"
#include "cxImageAlgorithms.h"
#include "cxImage.h"
#include "cxPatientModelService.h"

namespace cx
{

ExportDataDialog::ExportDataDialog(PatientModelServicePtr patientModelService, QWidget* parent) :
	QDialog(parent),
	mPatientModelService(patientModelService)
{
  this->setAttribute(Qt::WA_DeleteOnClose);

  QVBoxLayout* layout = new QVBoxLayout(this);
  this->setWindowTitle("Export Patient Data");

  mNiftiFormatCheckBox = new QCheckBox("Use RAS NIfTI-1/ITK-Snap axis definition", this);
  mNiftiFormatCheckBox->setToolTip(""
	  "Use RAS (X=Left->Right Y=Posterior->Anterior Z=Inferior->Superior), as in ITK-Snap.\n"
	  "This is different from LPS (DICOM).");
  mNiftiFormatCheckBox->setChecked(true);
  mNiftiFormatCheckBox->setEnabled(true);

  layout->addWidget(mNiftiFormatCheckBox);

  QHBoxLayout* buttons = new QHBoxLayout;
  layout->addLayout(buttons);
  mOkButton = new QPushButton("OK", this);
  buttons->addStretch();
  buttons->addWidget(mOkButton);
  connect(mOkButton, SIGNAL(clicked()), this, SLOT(accept()));
  connect(this, SIGNAL(accepted()), this, SLOT(acceptedSlot()));
  mOkButton->setDefault(true);
  mOkButton->setFocus();

//  report("Exporting data...");
}

ExportDataDialog::~ExportDataDialog()
{
}

PATIENT_COORDINATE_SYSTEM ExportDataDialog::getExternalSpace()
{
	return mNiftiFormatCheckBox->isChecked() ? pcsRAS : pcsLPS;
}

void ExportDataDialog::acceptedSlot()
{
	mPatientModelService->exportPatient(this->getExternalSpace());
}

}//namespace cx
