/*
 * cxLayoutEditorWidget.h
 *
 *  \date Jul 27, 2010
 *      \author christiana
 */

#ifndef CXLAYOUTEDITORWIDGET_H_
#define CXLAYOUTEDITORWIDGET_H_


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
 * \ingroup cxGUI
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

/**
 * \ingroup cxGUI
 *
 */
class LayoutEditorWidget : public QWidget
{
  Q_OBJECT
public:
  struct GridElement
  {
    QFrame* mFrame;
    QLabel* mLabel;
  };

  LayoutEditorWidget(QWidget* parent);
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
  void initCache();
  void clearDisplay();
  void updateGrid();
  LayoutData::ViewData getViewData(QPoint pt);
  void colorRegion(LayoutRegion region, QString selectColor, QString backColor);
  std::set<LayoutData::iterator> getSelectedViews();
  void updateSelection(QPoint pos);
  void setSaneGroupIDs();

  QBoxLayout* mTopLayout;
  QBoxLayout* mRCLayout;
  QGridLayout* mLayout;
  QSpinBox* mRowsEdit;
  QSpinBox* mColsEdit;
  QLineEdit* mNameEdit;

  std::vector<std::vector<GridElement> > mViewDataCache; ///< 10x10 grid of all available views
  LayoutData mViewData; ///< RxC grid of currently used views.
  std::map<ssc::PLANE_TYPE, QString> mPlaneNames; ///< names of planes for use in gui
  QPoint mClickPos;
  LayoutRegion mSelection;

  struct ViewNamesType
  {
    ViewNamesType() {}
    ViewNamesType(ssc::PLANE_TYPE plane, ssc::View::Type view, QString name) : mPlane(plane), mView(view), mName(name) {}
    ssc::PLANE_TYPE mPlane;
    ssc::View::Type mView;
    QString mName;
  };
  std::vector<ViewNamesType> mViewNames;
  QString getViewName(LayoutData::ViewData data) const;

  virtual void mouseMoveEvent(QMouseEvent* event);
  virtual void mousePressEvent(QMouseEvent* event);
};


} // namespace cx

#endif /* CXLAYOUTEDITORWIDGET_H_ */
