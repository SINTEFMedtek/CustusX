#ifndef CXIMPORTDATAWIZARD_H_
#define CXIMPORTDATAWIZARD_H_

#include <QDialog>
#include "cxDataInterface.h"
class QPushButton;
class QLabel;

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
//  ImportDataWizard(ssc::DataPtr data, QWidget* parent=NULL);
  ImportDataWizard(QString filename, QWidget* parent=NULL);
  ~ImportDataWizard();

public:
  void showEvent(QShowEvent* event);
private slots:
  void updateImportTransformButton();
  void importTransformSlot();
  void importDataSlot();
private:
  void setInitialGuessForParentFrame();
  ssc::DataPtr mData;
  QString mFilename;
  QLabel* mUidLabel;
  QLabel* mNameLabel;
  SetParentFrameStringDataAdapterPtr mParentFrameAdapter;
  QPushButton* mImportTransformButton;
  QWidget* mParentFrameCombo;
  QPushButton* mOkButton;

};
}//namespace cx
#endif /* CXIMPORTDATAWIZARD_H_ */
