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

namespace cx
{

ExportDataDialog::ExportDataDialog(QWidget* parent) :
    QDialog(parent)
{
  this->setAttribute(Qt::WA_DeleteOnClose);

  QVBoxLayout* layout = new QVBoxLayout(this);
  this->setWindowTitle("Export Patient Data");

  mNiftiFormatCheckBox = new QCheckBox("Use NIfTI-1/ITK-Snap axis definition", this);
  mNiftiFormatCheckBox->setToolTip(""
	  "Use X=Left->Right Y=Posterior->Anterior Z=Inferior->Superior, as in ITK-Snap.\n"
	  "This is different from normal DICOM.");
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

//  ssc::messageManager()->sendInfo("Exporting data...");
}

ExportDataDialog::~ExportDataDialog()
{
}

void ExportDataDialog::acceptedSlot()
{
	patientService()->getPatientData()->exportPatient(mNiftiFormatCheckBox->isChecked());
}

}//namespace cx
