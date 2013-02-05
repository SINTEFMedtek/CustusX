#ifndef CXIMPORTDATADIALOG_H_
#define CXIMPORTDATADIALOG_H_

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
 * \ingroup cxGUI
 *
 * \date Sep 24, 2010
 *\author christiana
 */
class ImportDataDialog : public QDialog
{
  Q_OBJECT
public:
  ImportDataDialog(QString filename, QWidget* parent=NULL);
  virtual ~ImportDataDialog();

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
  QLabel* mErrorLabel;
  SetParentFrameStringDataAdapterPtr mParentFrameAdapter;
  DataModalityStringDataAdapterPtr mModalityAdapter;
  ImageTypeStringDataAdapterPtr mImageTypeAdapter;
  QWidget* mParentFrameCombo;
  QWidget* mImageTypeCombo;
  QWidget* mModalityCombo;
  QPushButton* mOkButton;
  QCheckBox* mNiftiFormatCheckBox;
  QCheckBox* mTransformFromParentFrameCheckBox;
  QCheckBox* mConvertToUnsignedCheckBox;

  void convertFromNifti1Coordinates();
  void importParentTransform();
  void convertToUnsigned();
};
}//namespace cx

#endif /* CXIMPORTDATADIALOG_H_ */
