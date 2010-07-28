/*
 * cxLayoutEditor.cpp
 *
 *  Created on: Jul 27, 2010
 *      Author: christiana
 */

#include "cxLayoutEditor.h"
#include <iostream>
#include <QtGui>
#include "sscTypeConversions.h"
#include "sscDefinitionStrings.h"
#include "sscUtilHelpers.h"

namespace cx
{


LayoutEditor::LayoutEditor(QWidget* parent) :
  QWidget(parent)
{
  mTopLayout = new QVBoxLayout(this);
  QHBoxLayout* nameLayout = new QHBoxLayout;
  mTopLayout->addLayout(nameLayout);
  mRCLayout = new QHBoxLayout;
  mTopLayout->addLayout(mRCLayout);
  mLayout = new QGridLayout;
  mLayout->setMargin(0);
  mLayout->setSpacing(2);
  mTopLayout->addLayout(mLayout);

  mNameEdit = new QLineEdit;
  connect(mNameEdit, SIGNAL(editingFinished()), this, SLOT(nameChanged()));
  nameLayout->addWidget(new QLabel("Name"));
  nameLayout->addWidget(mNameEdit);

  // create the row/column bar
  mRowsEdit = new QSpinBox;
  mRowsEdit->setRange(1,10);
  mColsEdit = new QSpinBox;
  mColsEdit->setRange(1,10);
//  mColsEdit->setButtonSymbols(QAbstractSpinBox::NoButtons);
  connect(mRowsEdit, SIGNAL(valueChanged(int)), this, SLOT(rcChanged()));
  connect(mColsEdit, SIGNAL(valueChanged(int)), this, SLOT(rcChanged()));
//  connect(mRowsEdit, SIGNAL(editingFinished()), this, SLOT(rcChanged()));
//  connect(mColsEdit, SIGNAL(editingFinished()), this, SLOT(rcChanged()));
  mRCLayout->addWidget(new QLabel("Rows"));
  mRCLayout->addWidget(mRowsEdit);
  mRCLayout->addWidget(new QLabel("Columns"));
  mRCLayout->addWidget(mColsEdit);
  mRCLayout->addStretch();

  QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
  connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
  mTopLayout->addWidget(buttonBox);

  for (int i=ssc::ptNOPLANE; i<ssc::ptCOUNT; ++i)
  {
    ssc::PLANE_TYPE type = static_cast<ssc::PLANE_TYPE>(i);
    mPlaneNames[type] = qstring_cast(type);
  }
  mPlaneNames[ssc::ptNOPLANE] = "3D";

  initCache();

  this->updateGrid();
}

void LayoutEditor::setLayoutData(const LayoutData& data)
{
  mViewData = data;
  this->updateGrid();
}

LayoutData LayoutEditor::getLayoutData() const
{
  return mViewData;
}

void LayoutEditor::accept()
{
  std::cout << streamXml2String(mViewData) << std::endl;
}

void LayoutEditor::nameChanged()
{
  mViewData.setName(mNameEdit->text());
}

void LayoutEditor::contextMenuSlot(const QPoint& point)
{
  //QWidget* sender = dynamic_cast<QWidget*>(this->sender());
  QPoint pointGlobal = this->mapToGlobal(point);
  QMenu menu(this);

  LayoutData::ViewData viewData = this->getViewData(point);

  QAction* splitAction = new QAction("split view", &menu);
  splitAction->setEnabled(viewData.mRegion.span.row!=1 || viewData.mRegion.span.col!=1);
  connect(splitAction, SIGNAL(triggered()), this, SLOT(splitActionSlot()));
  menu.addAction(splitAction);

  menu.addSeparator();

  // actions for view group
  QActionGroup* groupActions = new QActionGroup(this);
  for (int i=0; i<3; ++i)
  {
    QAction* action = new QAction(QString("%1").arg(i), groupActions);
    action->setData(QVariant(i));
    action->setCheckable(true);
    connect(action, SIGNAL(triggered()), this, SLOT(groupActionSlot()));
    action->setChecked(viewData.mGroup==i);
//    menu.addAction(action);
  }

  menu.addMenu("View Group")->addActions(groupActions->actions());

  //menu.addActions(groupActions->actions());

  // actions for view type
  QActionGroup* typeActions = new QActionGroup(this);
  for (int i=ssc::ptNOPLANE; i<ssc::ptCOUNT; ++i)
  {
    ssc::PLANE_TYPE type = static_cast<ssc::PLANE_TYPE>(i);

    QAction* action = new QAction(QString("%1").arg(mPlaneNames[type]), typeActions);
    action->setData(QVariant(i));
    action->setCheckable(true);
    connect(action, SIGNAL(triggered()), this, SLOT(typeActionSlot()));
    action->setChecked(viewData.mPlane==type);
    //menu.addAction(action);
  }

  menu.addMenu("View Plane Type")->addActions(typeActions->actions());

  menu.exec(pointGlobal);
}

void LayoutEditor::splitActionSlot()
{
  QAction* sender = dynamic_cast<QAction*>(this->sender());
  if (!sender)
    return;
  LayoutData::ViewData viewData = this->getViewData(mClickPos);
  this->splitView(viewData.mRegion);
}

void LayoutEditor::groupActionSlot()
{
  QAction* sender = dynamic_cast<QAction*>(this->sender());
  if (!sender)
    return;
  LayoutData::ViewData viewData = this->getViewData(mClickPos);
  mViewData.get(viewData.mRegion.pos).mGroup = sender->data().toInt();
//  mViewData[viewData.mRegion.pos.row][viewData.mRegion.pos.col].mGroup = sender->data().toInt();
  this->updateGrid();
}

void LayoutEditor::typeActionSlot()
{
  QAction* sender = dynamic_cast<QAction*>(this->sender());
  if (!sender)
    return;
  LayoutData::ViewData viewData = this->getViewData(mClickPos);
  mViewData.get(viewData.mRegion.pos).mPlane = static_cast<ssc::PLANE_TYPE>(sender->data().toInt());
//  ViewData viewData = this->getViewData(mClickPos);
//  mViewData[viewData.mRegion.pos.row][viewData.mRegion.pos.col].mPlane = static_cast<ssc::PLANE_TYPE>(sender->data().toInt());
  this->updateGrid();
}

void LayoutEditor::mouseMoveEvent(QMouseEvent* event)
{
  LayoutData::ViewData start = this->getViewData(mClickPos);
  LayoutData::ViewData stop = this->getViewData(event->pos());
  LayoutRegion region = merge(start.mRegion, stop.mRegion);
  this->colorRegion(region, "dimgrey", "lightgrey");
}

void LayoutEditor::mousePressEvent(QMouseEvent* event)
{
  mClickPos = event->pos();

  if (event->button()==Qt::RightButton)
  {
    std::cout << "mouse press context" << std::endl;
    this->contextMenuSlot(event->pos());
    return;
  }

  std::cout << "mouse press clean" << std::endl;

  this->colorRegion(this->getViewData(mClickPos).mRegion, "dimgrey", "lightgrey");
}

void LayoutEditor::colorRegion(LayoutRegion region, QString selectColor, QString backColor)
{
  for (LayoutData::iterator iter=mViewData.begin(); iter!=mViewData.end(); ++iter)
  {
    LayoutPosition pos = iter->mRegion.pos;
    QString color;

    if (region.contains(pos))
      color = selectColor;
    else
      color = backColor;

    mViewDataCache[pos.row][pos.col].mFrame->setStyleSheet(QString("QFrame { background-color: %1 }").arg(color));
  }
}

/** Find view data for the frame under point pt
 *
 */
LayoutData::ViewData LayoutEditor::getViewData(QPoint pt)
{
  for (LayoutData::iterator iter=mViewData.begin(); iter!=mViewData.end(); ++iter)
  {
    LayoutPosition pos = iter->mRegion.pos;
    if (!mViewDataCache[pos.row][pos.col].mFrame->geometry().contains(pt))
      continue;

    return *iter;
  }

  return LayoutData::ViewData();
}

void LayoutEditor::mouseReleaseEvent(QMouseEvent* event)
{
  std::cout << "mouse release" << std::endl;
  LayoutData::ViewData start = this->getViewData(mClickPos);
  LayoutData::ViewData stop = this->getViewData(event->pos());
  LayoutRegion region = merge(start.mRegion, stop.mRegion);

  this->mergeViews(region);
  this->colorRegion(region, "lightgrey", "lightgrey");
}

/**called when row/column text boxes changed. Update model.
 *
 */
void LayoutEditor::rcChanged()
{
//  int rows = mRowsEdit->text().toInt();
//  int cols = mColsEdit->text().toInt();
//  rows = ssc::constrainValue(rows, 1, 10);
//  cols = ssc::constrainValue(cols, 1, 10);
//  this->resizeLayout(rows, cols);
  this->resizeLayout(mRowsEdit->value(), mColsEdit->value());
}

/** Merge all views from r1,c1 to and including r2,c2.
 *  (r1,c1) is the starting point. Previously merged views
 *  partially inside the new region will be split.
 *
 *  Prerequisite: input is inside old boundaries.
 */
void LayoutEditor::mergeViews(LayoutRegion region)
{
  mViewData.merge(region);
  this->updateGrid();
}

void LayoutEditor::splitView(LayoutRegion region)
{
  mViewData.split(region);
  this->updateGrid();
}

void LayoutEditor::resizeLayout(int rows, int cols)
{
  mViewData.resize(rows, cols);
  this->updateGrid();
  this->colorRegion(LayoutRegion(-1,-1,1,1), "lightgrey", "lightgrey");
}

/**Set visibility and position of frames in the gridlayout
 * according to the contents of mViewData.
 *
 */
void LayoutEditor::updateGrid()
{
  this->clearDisplay();

  //std::cout << "update start" << std::endl;
  for (LayoutData::iterator iter=mViewData.begin(); iter!=mViewData.end(); ++iter)
  {
    LayoutRegion region = iter->mRegion;
    GridElement gridData = mViewDataCache[region.pos.row][region.pos.col];

    // add and show frame in correct position
    mLayout->addWidget(gridData.mFrame, region.pos.row, region.pos.col, region.span.row, region.span.col);
    gridData.mFrame->show();

    // set view text
    if (iter->mGroup<0 && iter->mPlane==ssc::ptCOUNT)
      gridData.mLabel->setText("NA");
    else
      gridData.mLabel->setText(QString("%1/%2").arg(iter->mGroup).arg(mPlaneNames[iter->mPlane]));
  }

  mNameEdit->setText(mViewData.getName());
//  mRowsEdit->setText(qstring_cast(mViewData.size().row));
//  mColsEdit->setText(qstring_cast(mViewData.size().col));
  mRowsEdit->setValue(mViewData.size().row);
  mColsEdit->setValue(mViewData.size().col);

  this->colorRegion(LayoutRegion(-1,-1,1,1), "lightgrey", "lightgrey");

  this->updateGeometry();
  QSize msize = mLayout->minimumSize();
//  QSize rsize = mGridWidget->size();
  //std::cout << "minsize: " << msize.width() << "," << msize.height() << std::endl;
//  std::cout << "real size: " << rsize.width() << "," << rsize.height() << std::endl;

//  QString color("QFrame { background-color: red }");
//  mViewData[0][0].mFrame->setStyleSheet(color);

//  mGridWidget->resize(msize);
//  this->resize(400,700);
//  QTimer::singleShot(0, this, SLOT(setNiceSize()));
}

//void LayoutEditor::setNiceSize()
//{
//  //this->resize(400,700);
//}

void LayoutEditor::clearDisplay()
{
  for (int r = 0; r < mViewDataCache.size(); ++r)
  {
    for (int c = 0; c < mViewDataCache[r].size(); ++c)
    {
      mViewDataCache[r][c].mFrame->hide();
    }
  }
}

void LayoutEditor::initCache()
{
  int maxRows = 10;
  int maxCols = 10;
  mViewDataCache.resize(maxRows);

  for (int r = 0; r < maxRows; ++r)
  {
    mViewDataCache[r].resize(maxCols);

    for (int c = 0; c < maxCols; ++c)
    {
      QFrame* frame = new QFrame(this);
      //frame->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
      frame->setFrameStyle(QFrame::Panel | QFrame::Sunken);
      frame->setLineWidth(3);
      frame->setLayout(new QVBoxLayout);
      QLabel* label = new QLabel("NA", frame);
      frame->layout()->addWidget(label);

      mViewDataCache[r][c].mFrame = frame;
      mViewDataCache[r][c].mLabel = label;
    }
  }
}


} // namespace cx
