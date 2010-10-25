#ifndef CXIMPORTDATAWIZARD_H_
#define CXIMPORTDATAWIZARD_H_

#include <QDialog>
#include "cxDataInterface.h"
class QPushButton;

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
  ImportDataWizard(ssc::DataPtr data, QWidget* parent=NULL);
  ~ImportDataWizard();

private slots:
  void updateImportTransformButton();
  void importTransformSlot();
private:
  void setInitialGuessForParentFrame();
  ssc::DataPtr mData;
  ParentFrameStringDataAdapterPtr mParentFrameAdapter;
  QPushButton* mImportTransformButton;
};
}//namespace cx
#endif /* CXIMPORTDATAWIZARD_H_ */
