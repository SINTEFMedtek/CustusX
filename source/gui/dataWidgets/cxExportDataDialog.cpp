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
	return mNiftiFormatCheckBox->isChecked() ? pcsLPS : pcsRAS;
}

void ExportDataDialog::acceptedSlot()
{
	mPatientModelService->exportPatient(this->getExternalSpace());
}

}//namespace cx
