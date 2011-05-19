/*
 * sscFileSelectWidget.h
 *
 *  Created on: May 6, 2011
 *      Author: christiana
 */

#ifndef SSCFILESELECTWIDGET_H_
#define SSCFILESELECTWIDGET_H_

#include <QWidget>
#include <QStringList>

class QComboBox;
class QToolButton;
class QAction;

namespace ssc
{

/**Widget for displaying and selecting a single file.
 *
 * A combo box shows all available files within the
 * current path (recursively), and a tool button is
 * used to browse for files outside this path.
 *
 * Default name filter is .mhdmNameFilters
 *
 */
class FileSelectWidget : public QWidget
{
  Q_OBJECT

public:
  FileSelectWidget(QWidget* parent);
  QString getFilename() const;
  void setFilename(QString name);
  void setNameFilter(QStringList filter);
  void setPath(QString path);

public slots:
  void refresh();

signals:
  void fileSelected(QString name);

private slots:
  void selectData();
  void currentDataComboIndexChanged(int);
  void updateComboBox();

private:
  QStringList getAllFiles(QString folder);

  QComboBox* mDataComboBox;
  QToolButton* mSelectDataButton;
  QAction* mSelectDataAction;

  QString mFilename;
  QString mRootPath;
  QStringList mNameFilters;
};


}

#endif /* SSCFILESELECTWIDGET_H_ */
