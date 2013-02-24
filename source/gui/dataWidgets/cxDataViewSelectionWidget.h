/*
 * sscDataViewSelectionWidget.h
 *
 *  \date Apr 5, 2011
 *      \author christiana
 */

#ifndef CXDATAVIEWSELECTIONWIDGET_H_
#define CXDATAVIEWSELECTIONWIDGET_H_

#include <QListWidget>
//#include "cxViewWrapper.h"
#include "cxForwardDeclarations.h"
class QLabel;

namespace cx
{
/**
 * \file
 * \addtogroup cxGUI
 * @{
 */

/**
 * Base class for displaying ssc::Data items.
 */

class DataListWidget : public QListWidget
{
  Q_OBJECT

public:
  DataListWidget(QWidget* parent = NULL);
  virtual ~DataListWidget();
//	virtual QSize minimumSizeHint () const;
  virtual QSize sizeHint() const;

signals:
//  void dataSelected(QString uid);
  void userChangedList(); ///< emitted whenever the user changes the list
  void listSizeChanged(); ///< emitted whenever the count changes

protected:
  void populate(QStringList dataUids);
//  void populateData(QString uid);
  void populateData(QString uid, bool indent=false, QListWidgetItem* after = NULL);

//  Tool::InternalStructure getToolInternal(QString toolAbsoluteFilePath);

private slots:
//  void dataClickedSlot(QListWidgetItem* item);
  void itemSelectionChangedSlot();
};

//---------------------------------------------------------------------------------------------------------------------

class AllDataListWidget : public DataListWidget
{
  Q_OBJECT

public:
  AllDataListWidget(QWidget* parent = NULL);
  virtual ~AllDataListWidget();


public slots:
//  void filterSlot(QStringList applicationsFilter, QStringList trackingsystemsFilter);

protected:
  void mousePressEvent(QMouseEvent *event);
  void mouseMoveEvent(QMouseEvent *event);
//  void startDrag();

private slots:
  void populateAllDataList();

private:
//  QStringList getAbsoluteFilePathToAllTools(QDir dir); ///< get absolute file path to all tool.xml files in folder dir and all subfolders
//  QStringList filter(QStringList toolsToFilter, QStringList applicationsFilter, QStringList trackingsystemsFilter); ///<
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
//  void configSlot(QStringList toolsAbsoluteFilePath); ///< adds all input tools to the list
//  void filterSlot(QStringList trackingsystemFilter); ///< filters the tools on tracking system
  void populateList();
  void userChangedListSlot();

private slots:
  void deleteSlot();
  void deleteItemSlot(QListWidgetItem* item);
  void contextMenuSlot(const QPoint & point);
  void keyPressEvent(QKeyEvent* event);

protected:
  QMap<int, QVariant> convertFromCustomQtMimeFormat(const QMimeData* mimeData) const;

//  void dragEnterEvent(QDragEnterEvent *event);
//  void dragMoveEvent(QDragMoveEvent *event);
//  bool dropMimeData(int index, const QMimeData* data, Qt::DropAction action);

//  void mousePressEvent(QMouseEvent *event);
//  void mouseMoveEvent(QMouseEvent *event);
//  void startDrag();

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
//  void populateAllDataList();
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
