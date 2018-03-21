/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXIMPORTDATADIALOG_H_
#define CXIMPORTDATADIALOG_H_

#include "cxGuiExport.h"

#include <QDialog>
#include "cxDataInterface.h"
class QPushButton;
class QLabel;
class QCheckBox;

namespace cx
{
/**
 * \class ImportDataDialog
 *
 *\brief
 * \ingroup cx_gui
 *
 * \date Sep 24, 2010
 *\author christiana
 */
class cxGui_EXPORT ImportDataDialog : public QDialog
{
  Q_OBJECT
public:
  ImportDataDialog(PatientModelServicePtr patientModelService, ViewServicePtr viewService, QString filename, QWidget* parent=NULL);
  virtual ~ImportDataDialog();

public:
  void showEvent(QShowEvent* event);
private slots:
  void updateImportTransformButton();
  void importDataSlot();
  void acceptedSlot();
  void finishedSlot();
private:
  void setInitialGuessForParentFrame();
  DataPtr mData;
  QString mFilename;
  QLabel* mUidLabel;
  QLabel* mNameLabel;
  QLabel* mErrorLabel;
  StringPropertySetParentFramePtr mParentFrameAdapter;
  StringPropertyDataModalityPtr mModalityAdapter;
  StringPropertyImageTypePtr mImageTypeAdapter;
  QWidget* mParentFrameCombo;
  QWidget* mImageTypeCombo;
  QWidget* mModalityCombo;
  QPushButton* mOkButton;
  QCheckBox* mNiftiFormatCheckBox;
  QCheckBox* mTransformFromParentFrameCheckBox;
  QCheckBox* mConvertToUnsignedCheckBox;
  PatientModelServicePtr mPatientModelService;
  ViewServicePtr mViewService;

  void convertFromNifti1Coordinates();
  void importParentTransform();
  void convertToUnsigned();
};
}//namespace cx

#endif /* CXIMPORTDATADIALOG_H_ */
