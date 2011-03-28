#ifndef CXIMPORTDATAWIZARD_H_
#define CXIMPORTDATAWIZARD_H_

#include <QDialog>
#include "cxDataInterface.h"
class QPushButton;
class QLabel;
class QCheckBox;

namespace cx
{
/**
 * \class ImportDataWizard
 *
 *\brief
 *
 * \date Sep 24, 2010
 *\author christiana
 */
class ImportDataWizard : public QDialog
{
  Q_OBJECT
public:
  ImportDataWizard(QString filename, QWidget* parent=NULL);
  virtual ~ImportDataWizard();

public:
  void showEvent(QShowEvent* event);
private slots:
  void updateImportTransformButton();
  void importDataSlot();
  void acceptedSlot();
private:
  void setInitialGuessForParentFrame();
  ssc::DataPtr mData;
  QString mFilename;
  QLabel* mUidLabel;
  QLabel* mNameLabel;
  SetParentFrameStringDataAdapterPtr mParentFrameAdapter;
  QWidget* mParentFrameCombo;
  QPushButton* mOkButton;
  QCheckBox* mNiftiFormatCheckBox;
  QCheckBox* mTransformFromParentFrameCheckBox;

  void convertFromNifti1Coordinates();
  void importParentTransform();
};
}//namespace cx

#endif /* CXIMPORTDATAWIZARD_H_ */
