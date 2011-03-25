#ifndef CXFILEPREVIEWWIDGET_H_
#define CXFILEPREVIEWWIDGET_H_

#include "cxWhatsThisWidget.h"

class QTextDocument;
class QTextEdit;
class QPushButton;
class QFileSystemWatcher;
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
  void previewFileSlot(const QString& absoluteFilePath);
  void editSlot();

private:
  void watchFile(bool on);

  QTextDocument* mTextDocument;
  QTextEdit*     mTextEdit;

  QPushButton*   mEditButton;

  QFileSystemWatcher* mFileSystemWatcher;
  QFile*         mCurrentFile;
};

}

#endif /* CXFILEPREVIEWWIDGET_H_ */
