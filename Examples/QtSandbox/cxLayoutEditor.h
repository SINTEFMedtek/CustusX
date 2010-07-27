/*
 * cxLayoutEditor.h
 *
 *  Created on: Jul 27, 2010
 *      Author: christiana
 */

#ifndef CXLAYOUTEDITOR_H_
#define CXLAYOUTEDITOR_H_


#include <map>
#include <QWidget>
class QFrame;
class QLabel;
class QBoxLayout;
class QGridLayout;
class QLineEdit;
#include "cxLayoutData.h"

namespace cx
{

class LayoutEditor : public QWidget
{
  Q_OBJECT
public:
  struct GridElement
  {
    QFrame* mFrame;
    QLabel* mLabel;
  };
//   struct ViewData
//    {
//      ViewData() : mGroup(-1), mPlane(ssc::ptNOPLANE), mRegion(-1,-1,1,1), mFrame(NULL), mLabel(NULL) {}
//      ViewData(int row, int col, int rowSpan=1, int colSpan=1) : mGroup(-1), mPlane(ssc::ptNOPLANE), mRegion(row,col,rowSpan,colSpan) {}
//      int mGroup; ///< what group to connect to. -1 means not set.
//      ssc::PLANE_TYPE mPlane; ///< ptNOPLANE means 3D
//      //int mRow, mCol, mRowSpan, mColSpan;
//      LayoutRegion mRegion;
//
//      QFrame* mFrame;
//      QLabel* mLabel;
//    };

  LayoutEditor(QWidget* parent);

private slots:
  void rcChanged();
  void contextMenuSlot(const QPoint& point);
  void splitActionSlot();
  void groupActionSlot();
  void typeActionSlot();

  //void setNiceSize();
private:
  QBoxLayout* mTopLayout;
  QBoxLayout* mRCLayout;
  QGridLayout* mLayout;
  //QWidget* mGridWidget;
  QLineEdit* mRowsEdit;
  QLineEdit* mColsEdit;
  void initCache();
  void resizeLayout(int rows, int cols);
  void clearDisplay();
  void updateGrid();
  void mergeViews(LayoutRegion region);
  void splitView(LayoutRegion region);
  LayoutData::ViewData getViewData(QPoint pt);
  void colorRegion(LayoutRegion region, QString selectColor, QString backColor);

  std::vector<std::vector<GridElement> > mViewDataCache; ///< 10x10 grid of all available views
  LayoutData mViewData; ///< RxC grid of currently used views.
  std::map<ssc::PLANE_TYPE, QString> mPlaneNames; ///< names of planes for use in gui
  QPoint mClickPos;

  virtual void mouseMoveEvent(QMouseEvent* event);
  virtual void mousePressEvent(QMouseEvent* event);
  virtual void mouseReleaseEvent(QMouseEvent* event);
};


} // namespace cx

#endif /* CXLAYOUTEDITOR_H_ */
