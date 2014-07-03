/*
 * sscDataViewSelectionWidget.h
 *
 *  \date Apr 5, 2011
 *      \author christiana
 */

#ifndef CXDATAVIEWSELECTIONWIDGET_H_
#define CXDATAVIEWSELECTIONWIDGET_H_

#include <QListWidget>
#include "cxForwardDeclarations.h"
#include "cxLegacySingletons.h"
class QLabel;

namespace cx
{
/**
 * \file
 * \addtogroup cx_gui
 * @{
 */

/**
 * Base class for displaying Data items.
 */
class DataListWidget : public QListWidget
{
  Q_OBJECT

public:
  DataListWidget(QWidget* parent = NULL);
  virtual ~DataListWidget();
  virtual QSize sizeHint() const;

signals:
  void userChangedList(); ///< emitted whenever the user changes the list
  void listSizeChanged(); ///< emitted whenever the count changes

protected:
  void populate(QStringList dataUids);
  void populateData(QString uid, bool indent=false, QListWidgetItem* after = NULL);

private slots:
  void itemSelectionChangedSlot();
};

//---------------------------------------------------------------------------------------------------------------------

class AllDataListWidget : public DataListWidget
{
  Q_OBJECT

public:
  AllDataListWidget(QWidget* parent = NULL);
  virtual ~AllDataListWidget();

protected:
  void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);

private slots:
  void populateAllDataList();

private:
  QPoint startPos;
};

//---------------------------------------------------------------------------------------------------------------------

class SelectedDataListWidget : public DataListWidget
{
  Q_OBJECT

public:
  SelectedDataListWidget(QWidget* parent = NULL);
  virtual ~SelectedDataListWidget();
  void setViewGroupData(ViewGroupDataPtr viewGroupData);

  virtual void dropEvent(QDropEvent* event);
  QStringList getData(); ///< get absolute file path to all tools currently in the list

public slots:
  void populateList();
  void userChangedListSlot();

private slots:
  void deleteSlot();
  void deleteItemSlot(QListWidgetItem* item);
  void contextMenuSlot(const QPoint & point);
  void keyPressEvent(QKeyEvent* event);

protected:
	QMap<int, QVariant> convertFromCustomQtMimeFormat(const QMimeData* mimeData) const;

private:
  QListWidgetItem* mItemToDelete;
  ViewGroupDataPtr mViewGroupData;
  QPoint startPos;
};

/**Widget for selecting which data items to show in a given view group,
 * and how to order them.
 *
 */
class DataViewSelectionWidget : public QWidget
{
  Q_OBJECT
public:
  DataViewSelectionWidget(QWidget* parent = NULL);
  virtual ~DataViewSelectionWidget();

private slots:
  void viewGroupChangedSlot();

private:
  SelectedDataListWidget* mSelectedDataListWidget;
  AllDataListWidget* mAllDataListWidget;
  QLabel* mVisibleLabel;
};

/**
 * @}
 */
}

#endif /* CXDATAVIEWSELECTIONWIDGET_H_ */
