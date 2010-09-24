#ifndef CXIMPORTDATAWIZARD_H_
#define CXIMPORTDATAWIZARD_H_

#include <QDialog>
#include "cxDataInterface.h"

namespace cx
{
/*
 * ImportDataWizard
 *
 *  Created on: Sep 24, 2010
 *      Author: christiana
 */
class ImportDataWizard : public QDialog
{
  Q_OBJECT
public:
  ImportDataWizard(ssc::DataPtr data, QWidget* parent=NULL);
  ~ImportDataWizard();

private:
  ssc::DataPtr mData;
  ParentFrameStringDataAdapterPtr mParentFrameAdapter;
};
}//namespace cx
#endif /* CXIMPORTDATAWIZARD_H_ */
