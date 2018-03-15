/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXLAYOUTEDITORWIDGET_H_
#define CXLAYOUTEDITORWIDGET_H_

#include "cxGuiExport.h"

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
 * \ingroup cx_gui
 */
class cxGui_EXPORT SmallLineEdit : public QLineEdit
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
 * \ingroup cx_gui
 *
 */
class cxGui_EXPORT LayoutEditorWidget : public QWidget
{
  Q_OBJECT
public:
  struct GridElement
  {
    QFrame* mFrame;
    QLabel* mLabel;
  };

  LayoutEditorWidget(cx::ViewServicePtr viewService, QWidget* parent);
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
  LayoutViewData getViewData(QPoint pt);
  void colorRegion(LayoutRegion region, QString selectColor, QString backColor);
  std::set<LayoutData::iterator> getSelectedViews();
  void updateSelection(QPoint pos);
  void setSaneGroupIDs();
  QString getViewName(LayoutViewData data) const;
  virtual void mouseMoveEvent(QMouseEvent* event);
  virtual void mousePressEvent(QMouseEvent* event);
  void onOffScreenRenderingChanged();

  QBoxLayout* mTopLayout;
  QBoxLayout* mRCLayout;
  QGridLayout* mLayout;
  QSpinBox* mRowsEdit;
  QSpinBox* mColsEdit;
  QLineEdit* mNameEdit;
  BoolPropertyPtr mOffScreenRendering;

  std::vector<std::vector<GridElement> > mViewDataCache; ///< LayoutData::MaxGridSize^2 grid of all available views
  LayoutData mViewData; ///< RxC grid of currently used views.
  std::map<PLANE_TYPE, QString> mPlaneNames; ///< names of planes for use in gui
  QPoint mClickPos;
  LayoutRegion mSelection;
  ViewServicePtr mViewService;

  struct ViewNamesType
  {
    ViewNamesType() {}
    ViewNamesType(PLANE_TYPE plane, View::Type view, QString name) : mPlane(plane), mView(view), mName(name) {}
    PLANE_TYPE mPlane;
    View::Type mView;
    QString mName;
  };
  std::vector<ViewNamesType> mViewNames;

};


} // namespace cx

#endif /* CXLAYOUTEDITORWIDGET_H_ */
