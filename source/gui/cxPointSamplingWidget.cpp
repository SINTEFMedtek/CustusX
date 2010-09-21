/*
 * cxToolWidget.cpp
 *
 *  Created on: Apr 22, 2010
 *      Author: christiana
 */

#include "cxPointSamplingWidget.h"

#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QStringList>
#include <QVBoxLayout>

#include "cxToolManager.h"
#include "sscTypeConversions.h"

namespace cx
{

PointSamplingWidget::PointSamplingWidget(QWidget* parent) :
  QWidget(parent),
  mVerticalLayout(new QVBoxLayout(this)),
  mTable(new QTableWidget(this)),
  mAddButton(new QPushButton("Add", this)),
  mEditButton(new QPushButton("Resample", this)),
  mRemoveButton(new QPushButton("Remove", this))
{
  this->setObjectName("PointSamplingWidget");
  this->setWindowTitle("PointSampling");

  //table widget
  connect(mTable, SIGNAL(itemSelectionChanged()), this, SLOT(itemSelectionChanged()));

  this->setLayout(mVerticalLayout);

  //pushbuttons
  connect(mAddButton, SIGNAL(clicked()), this, SLOT(addButtonClickedSlot()));
  mEditButton->setDisabled(true);
  connect(mEditButton, SIGNAL(clicked()), this, SLOT(editButtonClickedSlot()));
  mRemoveButton->setDisabled(true);
  connect(mRemoveButton, SIGNAL(clicked()), this, SLOT(removeButtonClickedSlot()));

  //layout
  mVerticalLayout->addWidget(mTable);

  QHBoxLayout* buttonLayout = new QHBoxLayout;
  mVerticalLayout->addLayout(buttonLayout);

  buttonLayout->addWidget(mAddButton);
  buttonLayout->addWidget(mEditButton);
  buttonLayout->addWidget(mRemoveButton);
}

void PointSamplingWidget::itemSelectionChanged()
{
  std::cout << "pling" << std::endl;

  QTableWidgetItem* item = mTable->currentItem();

  mActiveLandmark = string_cast(item->data(Qt::UserRole).toString());

  for (unsigned i=0; i<mSamples.size(); ++i)
  {
    if (mSamples[i].getUid()!=mActiveLandmark)
      continue;
    setManualTool(mSamples[i].getCoord());
    break;
  }

  enablebuttons();
}

PointSamplingWidget::~PointSamplingWidget()
{
}

void PointSamplingWidget::showEvent(QShowEvent* event)
{
  QWidget::showEvent(event);
  this->updateSlot();
}

void PointSamplingWidget::updateSlot()
{
  mTable->blockSignals(true);
  mTable->clear();

  //ready the table widget
  mTable->setRowCount(mSamples.size());
  mTable->setColumnCount(3);
  QStringList headerItems(QStringList() << "Name" << "Coordinates(r)" << "Delta (mm)");
  mTable->setHorizontalHeaderLabels(headerItems);
  mTable->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
  mTable->setSelectionBehavior(QAbstractItemView::SelectRows);

  for (unsigned i = 0; i < mSamples.size(); ++i)
  {
    std::vector<QTableWidgetItem*> items(3); // name, coordinates, delta

    ssc::Vector3D coord = mSamples[i].getCoord();

    items[0] = new QTableWidgetItem(qstring_cast(mSamples[i].getUid()));

    QString coordText;
    int width = 5;
    int prec = 1;
    coordText = tr("(%1, %2, %3)").arg(coord[0], width, 'f', prec).arg(coord[1], width, 'f', prec).arg(coord[2],
        width, 'f', prec);

    items[1] = new QTableWidgetItem(coordText);

    if (i==0)
    {
      items[2] = new QTableWidgetItem;
    }
    else
    {
      double delta = (mSamples[i].getCoord() - mSamples[0].getCoord()).length();
      items[2] = new QTableWidgetItem(tr("%1").arg(delta, width, 'f', prec));
    }

    for (unsigned j = 0; j < items.size(); ++j)
    {
      items[j]->setData(Qt::UserRole, qstring_cast(mSamples[i].getUid()));
      mTable->setItem(i, j, items[j]);
    }

    //highlight selected row
    if (mSamples[i].getUid() == mActiveLandmark)
    {
      mTable->setCurrentItem(items[1]);
    }
  }

  mTable->blockSignals(false);

  enablebuttons();
}

void PointSamplingWidget::enablebuttons()
{
  mAddButton->setEnabled(true);
  mEditButton->setEnabled(mActiveLandmark!="");
  mRemoveButton->setEnabled(mActiveLandmark!="");
}

void PointSamplingWidget::setManualTool(const ssc::Vector3D& p_r)
{
  ssc::ManualToolPtr tool = ToolManager::getInstance()->getManualTool();

  //ssc::Transform3D sMr = mSliceProxy->get_sMr();
  ssc::Transform3D rMpr = *ToolManager::getInstance()->get_rMpr();
  ssc::Transform3D prMt = tool->get_prMt();

  // find tool position in r
  ssc::Vector3D tool_t(0,0,tool->getTooltipOffset());
  ssc::Vector3D tool_r = (rMpr*prMt).coord(tool_t);

  // find click position in s.
  //ssc::Vector3D click_s = get_vpMs().inv().coord(click_vp);

  // compute the new tool position in slice space as a synthesis of the plane part of click and the z part of original.
  //ssc::Vector3D cross_s(click_s[0], click_s[1], tool_s[2]);
  // compute the position change and transform to patient.
  ssc::Vector3D delta_r = p_r - tool_r;
  ssc::Vector3D delta_pr = rMpr.inv().vector(delta_r);

  // MD is the actual tool movement in patient space, matrix form
  ssc::Transform3D MD = createTransformTranslate(delta_pr);
  // set new tool position to old modified by MD:
  tool->set_prMt(MD*prMt);
}

void PointSamplingWidget::addButtonClickedSlot()
{
  // find unique uid:
  int max = 0;
  for (unsigned i=0; i<mSamples.size(); ++i)
  {
    max = std::max(max, qstring_cast(mSamples[i].getUid()).toInt());
  }
  std::string uid = string_cast(max+1);

  mSamples.push_back(ssc::Landmark(uid, getSample()));
  mActiveLandmark = uid;

  updateSlot();
}

ssc::Vector3D PointSamplingWidget::getSample() const
{
  // find current tool position:
  ssc::ToolPtr tool = ssc::toolManager()->getDominantTool();
  if (!tool)
    return ssc::Vector3D(0,0,0);
  ssc::Transform3D prMt = tool->get_prMt();
  ssc::Transform3D rMpr = *ssc::toolManager()->get_rMpr();
  ssc::Vector3D pos = (rMpr*prMt).coord(ssc::Vector3D(0,0,tool->getTooltipOffset()));

  return pos;
}

void PointSamplingWidget::editButtonClickedSlot()
{
  for (unsigned i=0; i<mSamples.size(); ++i)
  {
    if (mSamples[i].getUid()!=mActiveLandmark)
      continue;
    mSamples[i] = ssc::Landmark(mActiveLandmark, getSample());
  }
  updateSlot();
}

void PointSamplingWidget::removeButtonClickedSlot()
{
  for (unsigned i=0; i<mSamples.size(); ++i)
  {
    if (mSamples[i].getUid()!=mActiveLandmark)
      continue;
    mSamples.erase(mSamples.begin()+i);
    mActiveLandmark = "";
    if (i<mSamples.size())
      mActiveLandmark = mSamples[i].getUid();
    break;
  }
  updateSlot();
}

void PointSamplingWidget::gotoButtonClickedSlot()
{

}


}//end namespace cx
