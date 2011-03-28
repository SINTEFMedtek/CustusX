#ifndef CXFILEPREVIEWWIDGET_H_
#define CXFILEPREVIEWWIDGET_H_

#include "cxWhatsThisWidget.h"
#include "boost/shared_ptr.hpp"
class QTextDocument;
class QTextEdit;
class QFile;

namespace cx
{
/**
 *\class FilePreviewWidget
 *
 * \brief
 *
 * \date Mar 22, 2011
 * \author Janne Beate Bakeng, SINTEF
 */

class FilePreviewWidget : public WhatsThisWidget
{
  Q_OBJECT

public:
  FilePreviewWidget(QWidget* parent);
  virtual ~FilePreviewWidget();

  virtual QString defaultWhatsThis() const;

public slots:
  void previewFileSlot(QString absoluteFilePath);

private:
  QTextDocument* mTextDocument;
  QTextEdit*     mTextEdit;

  boost::shared_ptr<QFile>         mCurrentFile;
};

}

#endif /* CXFILEPREVIEWWIDGET_H_ */
