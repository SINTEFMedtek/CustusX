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
#include "cxViewManager.h"

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
  connect(mRowsEdit, SIGNAL(valueChanged(int)), this, SLOT(rowsColumnsChangedSlot()));
  connect(mColsEdit, SIGNAL(valueChanged(int)), this, SLOT(rowsColumnsChangedSlot()));
  mRCLayout->addWidget(new QLabel("Rows"));
  mRCLayout->addWidget(mRowsEdit);
  mRCLayout->addWidget(new QLabel("Columns"));
  mRCLayout->addWidget(mColsEdit);
  mRCLayout->addStretch();

  for (int i=ssc::ptNOPLANE; i<ssc::ptCOUNT; ++i)
  {
    ssc::PLANE_TYPE type = static_cast<ssc::PLANE_TYPE>(i);
    mPlaneNames[type] = qstring_cast(type);
    mViewNames.push_back(ViewNamesType(type,ssc::View::VIEW_2D, qstring_cast(type)));
  }
  mViewNames.push_back(ViewNamesType(ssc::ptNOPLANE,ssc::View::VIEW_3D, "3D"));
  mViewNames.push_back(ViewNamesType(ssc::ptNOPLANE,ssc::View::VIEW_REAL_TIME, "RT"));
//  mPlaneNames[ssc::ptNOPLANE] = "3D";
//  mPlaneNames[static_cast<ssc::PLANE_TYPE>(-1)] = "RT";

  mSelection = LayoutRegion(-1,-1);
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

  QAction* mergeAction = new QAction("merge view", &menu);
  mergeAction->setEnabled(this->getSelectedViews().size()>1);
  connect(mergeAction, SIGNAL(triggered()), this, SLOT(mergeActionSlot()));
  menu.addAction(mergeAction);

  QAction* splitAction = new QAction("split view", &menu);
  splitAction->setEnabled(mSelection.span.row!=1 || mSelection.span.col!=1);
  connect(splitAction, SIGNAL(triggered()), this, SLOT(splitActionSlot()));
  menu.addAction(splitAction);

  menu.addSeparator();

  // actions for view group
  int viewGroupCount = viewManager()->getViewGroups().size();
  QActionGroup* groupActions = new QActionGroup(this);
  for (int i=0; i<viewGroupCount; ++i)
  {
    QAction* action = new QAction(QString("Group %1").arg(i), groupActions);
    action->setData(QVariant(i));
    action->setCheckable(true);
    connect(action, SIGNAL(triggered()), this, SLOT(groupActionSlot()));
    action->setChecked(viewData.mGroup==i);
//    menu.addAction(action);
  }

  //menu.addMenu("View Group")->addActions(groupActions->actions());
  menu.addActions(groupActions->actions());
  menu.addSeparator();

  // actions for view type
  QActionGroup* typeActions = new QActionGroup(this);
//  for (std::map<ssc::PLANE_TYPE, QString>::iterator iter=mPlaneNames.begin(); iter!=mPlaneNames.end(); ++iter)
  for (unsigned i=0; i<mViewNames.size(); ++i)
  {
    ViewNamesType current = mViewNames[i];
//    ssc::PLANE_TYPE type = static_cast<ssc::PLANE_TYPE>(i);
//    ssc::PLANE_TYPE type = iter->first;
//    QString name = iter->second;

    QAction* action = new QAction(QString("%1").arg(current.mName), typeActions);
//    action->setData(QVariant(t));
    action->setCheckable(true);
    connect(action, SIGNAL(triggered()), this, SLOT(typeActionSlot()));
    action->setChecked(viewData.mPlane==current.mPlane && viewData.mType==current.mView);
  }
//  for (int i=ssc::ptNOPLANE; i<ssc::ptCOUNT; ++i)
//  {
//    ssc::PLANE_TYPE type = static_cast<ssc::PLANE_TYPE>(i);
//
//    QAction* action = new QAction(QString("%1").arg(mPlaneNames[type]), typeActions);
//    action->setData(QVariant(i));
//    action->setCheckable(true);
//    connect(action, SIGNAL(triggered()), this, SLOT(typeActionSlot()));
//    action->setChecked(viewData.mPlane==type);
//    //menu.addAction(action);
//  }

  //menu.addMenu("View Plane Type")->addActions(typeActions->actions());
  menu.addActions(typeActions->actions());

  menu.exec(pointGlobal);
}

void LayoutEditor::splitActionSlot()
{
  mViewData.split(mSelection);
  this->updateGrid();
}

void LayoutEditor::mergeActionSlot()
{
  mViewData.merge(mSelection);
  this->updateGrid();
}

void LayoutEditor::groupActionSlot()
{
  QAction* sender = dynamic_cast<QAction*>(this->sender());
  if (!sender)
    return;
  int group = sender->data().toInt();

  std::set<LayoutData::iterator> selection = this->getSelectedViews();
  for (std::set<LayoutData::iterator>::iterator iter=selection.begin(); iter!=selection.end(); ++iter)
    (*iter)->mGroup = group;

  this->updateGrid();
}

void LayoutEditor::typeActionSlot()
{
  QAction* sender = dynamic_cast<QAction*>(this->sender());
  if (!sender)
    return;
//  ssc::PLANE_TYPE type = static_cast<ssc::PLANE_TYPE>(sender->data().toInt());
  ViewNamesType type;
  for (unsigned i=0; i<mViewNames.size(); ++i)
    if (mViewNames[i].mName == sender->text())
      type=mViewNames[i];

  std::set<LayoutData::iterator> selection = this->getSelectedViews();
  for (std::set<LayoutData::iterator>::iterator iter=selection.begin(); iter!=selection.end(); ++iter)
  {
    (*iter)->mPlane = type.mPlane;
    (*iter)->mType = type.mView;
  }

  this->updateGrid();
}

void LayoutEditor::mouseMoveEvent(QMouseEvent* event)
{
  this->updateSelection(event->pos());
}

void LayoutEditor::updateSelection(QPoint pos)
{
  LayoutData::ViewData start = this->getViewData(mClickPos);
  LayoutData::ViewData stop = this->getViewData(pos);
  mSelection = merge(start.mRegion, stop.mRegion);
  this->colorRegion(mSelection, "dimgrey", "lightgrey");
}

/* Return a set of unique iterators into the layout data,
 * representing the selected region.
 */
std::set<LayoutData::iterator> LayoutEditor::getSelectedViews()
{
  std::set<LayoutData::iterator> retval;
  for (int r=mSelection.pos.row; r<mSelection.pos.row+mSelection.span.row; ++r)
    for (int c=mSelection.pos.col; c<mSelection.pos.col+mSelection.span.col; ++c)
      retval.insert(mViewData.find(LayoutPosition(r,c)));
  return retval;
}

void LayoutEditor::mousePressEvent(QMouseEvent* event)
{
  mClickPos = event->pos();

  if (event->button()==Qt::RightButton)
  {
    // reselect if click is outside old selection
    if (!mSelection.contains(this->getViewData(mClickPos).mRegion.pos))
      this->updateSelection(event->pos());

    //std::cout << "mouse press context" << std::endl;
    this->contextMenuSlot(event->pos());
  }
  else
  {
    //std::cout << "mouse press clean" << std::endl;
    this->updateSelection(event->pos());
  }
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

/**called when row/column text boxes changed. Update model.
 *
 */
void LayoutEditor::rowsColumnsChangedSlot()
{
  mViewData.resize(mRowsEdit->value(), mColsEdit->value());
  this->updateGrid();
}

QString LayoutEditor::getViewName(LayoutData::ViewData data) const
{
  for (unsigned i=0; i<mViewNames.size(); ++i)
  {
    if (mViewNames[i].mPlane==data.mPlane && mViewNames[i].mView==data.mType)
      return mViewNames[i].mName;
  }
  return "NA";
}

/**Set visibility and position of frames in the gridlayout
 * according to the contents of mViewData.
 *
 */
void LayoutEditor::updateGrid()
{
  //std::cout << "pre update:" << streamXml2String(mViewData) << std::endl;

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
    QString name = this->getViewName(*iter);
    if (iter->mGroup<0 && name.isEmpty())
      gridData.mLabel->setText("NA");
    else
      gridData.mLabel->setText(QString("%1/%2").arg(iter->mGroup).arg(name));
  }

  mNameEdit->setText(mViewData.getName());
//  mRowsEdit->setText(qstring_cast(mViewData.size().row));
//  mColsEdit->setText(qstring_cast(mViewData.size().col));

  mRowsEdit->blockSignals(true);
  mRowsEdit->setValue(mViewData.size().row);
  mRowsEdit->blockSignals(false);

  mColsEdit->blockSignals(true);
  mColsEdit->setValue(mViewData.size().col);
  mColsEdit->blockSignals(false);

//  this->colorRegion(LayoutRegion(-1,-1,1,1), "lightgrey", "lightgrey");
  this->colorRegion(mSelection, "dimgrey", "lightgrey");

  //this->updateGeometry();
  //QSize msize = mLayout->minimumSize();
//  QSize rsize = mGridWidget->size();
  //std::cout << "minsize: " << msize.width() << "," << msize.height() << std::endl;
//  std::cout << "real size: " << rsize.width() << "," << rsize.height() << std::endl;

//  QString color("QFrame { background-color: red }");
//  mViewData[0][0].mFrame->setStyleSheet(color);

//  mGridWidget->resize(msize);
//  this->resize(400,700);
//  QTimer::singleShot(0, this, SLOT(setNiceSize()));

 // std::cout << "post update:" << streamXml2String(mViewData) << std::endl;

}

//void LayoutEditor::setNiceSize()
//{
//  //this->resize(400,700);
//}

void LayoutEditor::clearDisplay()
{
  for (unsigned r = 0; r < mViewDataCache.size(); ++r)
  {
    for (unsigned c = 0; c < mViewDataCache[r].size(); ++c)
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
