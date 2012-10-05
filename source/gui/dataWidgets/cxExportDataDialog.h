#ifndef CXExportDataDialog_H_
#define CXExportDataDialog_H_

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
 * \ingroup cxGUI
 *
 * \date Oct 03, 2012
 *\author christiana
 */
class ExportDataDialog : public QDialog
{
  Q_OBJECT
public:
  ExportDataDialog(QWidget* parent=NULL);
  virtual ~ExportDataDialog();

private slots:
  void acceptedSlot();
private:
  QPushButton* mOkButton;
  QCheckBox* mNiftiFormatCheckBox;
};
}//namespace cx

#endif /* CXExportDataDialog_H_ */
