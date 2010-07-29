/*
 * cxLayoutEditor.h
 *
 *  Created on: Jul 27, 2010
 *      Author: christiana
 */

#ifndef CXLAYOUTEDITOR_H_
#define CXLAYOUTEDITOR_H_


#include <map>
#include <set>
#include <QWidget>
#include <QLineEdit>
class QFrame;
class QSpinBox;
class QLabel;
class QBoxLayout;
class QGridLayout;
class QLineEdit;
#include "cxLayoutData.h"

namespace cx
{


/**A QLineEdit
 */
class SmallLineEdit : public QLineEdit
{
public:
  SmallLineEdit(QWidget* parent = 0) : QLineEdit(parent) {}
  virtual QSize sizeHint() const
  {
    QSize size = QLineEdit::minimumSizeHint();
    size.setWidth(size.height()*2);
    return size;
  }
};


class LayoutEditor : public QWidget
{
  Q_OBJECT
public:
  struct GridElement
  {
    QFrame* mFrame;
    QLabel* mLabel;
  };

  LayoutEditor(QWidget* parent);
  void setLayoutData(const LayoutData& data);
  LayoutData getLayoutData() const;

private slots:
  void rowsColumnsChangedSlot();
  void contextMenuSlot(const QPoint& point);
  void splitActionSlot();
  void mergeActionSlot();
  void groupActionSlot();
  void typeActionSlot();
  void nameChanged();

private:
  QBoxLayout* mTopLayout;
  QBoxLayout* mRCLayout;
  QGridLayout* mLayout;
  QSpinBox* mRowsEdit;
  QSpinBox* mColsEdit;
  QLineEdit* mNameEdit;
  void initCache();
  void clearDisplay();
  void updateGrid();
  LayoutData::ViewData getViewData(QPoint pt);
  void colorRegion(LayoutRegion region, QString selectColor, QString backColor);
  std::set<LayoutData::iterator> getSelectedViews();
  void updateSelection(QPoint pos);

  std::vector<std::vector<GridElement> > mViewDataCache; ///< 10x10 grid of all available views
  LayoutData mViewData; ///< RxC grid of currently used views.
  std::map<ssc::PLANE_TYPE, QString> mPlaneNames; ///< names of planes for use in gui
  QPoint mClickPos;
  LayoutRegion mSelection;

  virtual void mouseMoveEvent(QMouseEvent* event);
  virtual void mousePressEvent(QMouseEvent* event);
};


} // namespace cx

#endif /* CXLAYOUTEDITOR_H_ */
