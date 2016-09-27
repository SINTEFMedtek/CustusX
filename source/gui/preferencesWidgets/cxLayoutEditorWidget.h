/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
#include "cxLegacySingletons.h"

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
  LayoutViewData getViewData(QPoint pt);
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
  BoolPropertyPtr mOffScreenRendering;

  std::vector<std::vector<GridElement> > mViewDataCache; ///< 10x10 grid of all available views
  LayoutData mViewData; ///< RxC grid of currently used views.
  std::map<PLANE_TYPE, QString> mPlaneNames; ///< names of planes for use in gui
  QPoint mClickPos;
  LayoutRegion mSelection;

  struct ViewNamesType
  {
    ViewNamesType() {}
    ViewNamesType(PLANE_TYPE plane, View::Type view, QString name) : mPlane(plane), mView(view), mName(name) {}
    PLANE_TYPE mPlane;
    View::Type mView;
    QString mName;
  };
  std::vector<ViewNamesType> mViewNames;
  QString getViewName(LayoutViewData data) const;

  virtual void mouseMoveEvent(QMouseEvent* event);
  virtual void mousePressEvent(QMouseEvent* event);
  void onOffScreenRenderingChanged();
};


} // namespace cx

#endif /* CXLAYOUTEDITORWIDGET_H_ */
