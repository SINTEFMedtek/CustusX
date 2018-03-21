/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXExportDataDialog_H_
#define CXExportDataDialog_H_

#include "cxGuiExport.h"

#include <QDialog>
#include "cxDataInterface.h"
class QPushButton;
class QLabel;
class QCheckBox;

namespace cx
{
/**
 * \class ExportDataDialog
 *
 *\brief
 * \ingroup cx_gui
 *
 * \date Oct 03, 2012
 *\author christiana
 */
class cxGui_EXPORT ExportDataDialog : public QDialog
{
  Q_OBJECT
public:
  ExportDataDialog(PatientModelServicePtr patientModelService, QWidget* parent=NULL);
  virtual ~ExportDataDialog();

private slots:
  void acceptedSlot();
private:
  QPushButton* mOkButton;
  QCheckBox* mNiftiFormatCheckBox;
  PatientModelServicePtr mPatientModelService;
  PATIENT_COORDINATE_SYSTEM getExternalSpace();
};
}//namespace cx

#endif /* CXExportDataDialog_H_ */
