
#include "cxPointSamplingWidget.h"

#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QStringList>
#include <QVBoxLayout>
#include <QHeaderView>

#include "sscMessageManager.h"
#include "sscTypeConversions.h"
#include "sscCoordinateSystemHelpers.h"
#include "cxToolManager.h"
#include "cxViewManager.h"
#include "cxViewGroup.h"
#include "cxViewWrapper.h"
#include "sscDataManager.h"
#include "sscManualTool.h"

namespace cx
{

PointSamplingWidget::PointSamplingWidget(QWidget* parent) :
  BaseWidget(parent, "PointSamplingWidget", "Point sampler/3D ruler"),
  mVerticalLayout(new QVBoxLayout(this)),
  mTable(new QTableWidget(this)),
  mActiveLandmark(""),
  mAddButton(new QPushButton("Add", this)),
  mEditButton(new QPushButton("Resample", this)),
  mRemoveButton(new QPushButton("Remove", this)),
  mLoadReferencePointsButton(new QPushButton("Load reference points", this))
{
  connect(ssc::toolManager(), SIGNAL(configured()), this, SLOT(updateSlot()));

  //table widget
  connect(mTable, SIGNAL(itemSelectionChanged()), this, SLOT(itemSelectionChanged()));

  this->setLayout(mVerticalLayout);

  //pushbuttons
  connect(mAddButton, SIGNAL(clicked()), this, SLOT(addButtonClickedSlot()));
  mEditButton->setDisabled(true);
  connect(mEditButton, SIGNAL(clicked()), this, SLOT(editButtonClickedSlot()));
  mRemoveButton->setDisabled(true);
  connect(mRemoveButton, SIGNAL(clicked()), this, SLOT(removeButtonClickedSlot()));
  connect(mLoadReferencePointsButton, SIGNAL(clicked()), this, SLOT(loadReferencePointsSlot()));

  //layout
  mVerticalLayout->addWidget(mTable);

  QHBoxLayout* buttonLayout = new QHBoxLayout;
  mVerticalLayout->addLayout(buttonLayout);

  buttonLayout->addWidget(mAddButton);
  buttonLayout->addWidget(mEditButton);
  buttonLayout->addWidget(mRemoveButton);
  mVerticalLayout->addWidget(mLoadReferencePointsButton);
}

PointSamplingWidget::~PointSamplingWidget()
{}

QString PointSamplingWidget::defaultWhatsThis() const
{
  return "<html>"
      "<h3>Utility for sampling points in 3D</h3>"
      "<p>Lets you sample points in 3D and get the distance between sampled points.</p>"
      "<p><i></i></p>"
      "</html>";
}

void PointSamplingWidget::itemSelectionChanged()
{
  //std::cout << "pling" << std::endl;

  QTableWidgetItem* item = mTable->currentItem();

  mActiveLandmark = item->data(Qt::UserRole).toString();

  for (unsigned i=0; i<mSamples.size(); ++i)
  {
    if (mSamples[i].getUid()!=mActiveLandmark)
      continue;
    setManualTool(mSamples[i].getCoord());
    break;
  }

  enablebuttons();
}

void PointSamplingWidget::showEvent(QShowEvent* event)
{
  QWidget::showEvent(event);

  ViewGroupDataPtr data = viewManager()->getViewGroups()[0]->getData();
  ViewGroupData::Options options = data->getOptions();
  options.mShowPointPickerProbe = true;
  data->setOptions(options);

  this->updateSlot();
}

void PointSamplingWidget::hideEvent(QHideEvent* event)
{
  QWidget::hideEvent(event);
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

  this->enablebuttons();
}

void PointSamplingWidget::enablebuttons()
{
  mAddButton->setEnabled(true);
  mEditButton->setEnabled(mActiveLandmark!="");
  mRemoveButton->setEnabled(mActiveLandmark!="");
  mLoadReferencePointsButton->setEnabled(ssc::toolManager()->getReferenceTool());
}

void PointSamplingWidget::addPoint(ssc::Vector3D point)
{
  // find unique uid:
  int max = 0;
  for (unsigned i=0; i<mSamples.size(); ++i)
  {
    max = std::max(max, qstring_cast(mSamples[i].getUid()).toInt());
  }
  QString uid = qstring_cast(max+1);

  mSamples.push_back(ssc::Landmark(uid, point));
  mActiveLandmark = uid;

  this->updateSlot();
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
  ssc::Transform3D MD = ssc::createTransformTranslate(delta_pr);
  // set new tool position to old modified by MD:
  tool->set_prMt(MD*prMt);
}

void PointSamplingWidget::addButtonClickedSlot()
{
  this->addPoint(this->getSample());
}

ssc::Vector3D PointSamplingWidget::getSample() const
{
// find current tool position:
//  ssc::ToolPtr tool = ssc::toolManager()->getDominantTool();
//  if (!tool)
//    return ssc::Vector3D(0,0,0);
//  ssc::Transform3D prMt = tool->get_prMt();
//  ssc::Transform3D rMpr = *ssc::toolManager()->get_rMpr();
//  ssc::Vector3D pos = (rMpr*prMt).coord(ssc::Vector3D(0,0,tool->getTooltipOffset()));

  ssc::CoordinateSystem ref = ssc::CoordinateSystemHelpers::getR();
  ssc::Vector3D P_ref = ssc::CoordinateSystemHelpers::getDominantToolTipPoint(ref, true);

  return P_ref;
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

void PointSamplingWidget::loadReferencePointsSlot()
{
  ssc::ToolPtr refTool = ssc::toolManager()->getReferenceTool();
  if(!refTool) // we only load reference points from reference tools
  {
    ssc::messageManager()->sendDebug("No reference tool, cannot load reference points into the pointsampler");
    return;
  }

  std::map<int, ssc::Vector3D> referencePoints_s = refTool->getReferencePoints();
  if(referencePoints_s.empty())
  {
    ssc::messageManager()->sendWarning("No referenceppoints in reference tool "+refTool->getName());
    return;
  }

  ssc::CoordinateSystem ref = ssc::CoordinateSystemHelpers::getR();
  ssc::CoordinateSystem sensor = ssc::CoordinateSystemHelpers::getS(refTool);

  std::map<int, ssc::Vector3D>::iterator it = referencePoints_s.begin();
  for(; it != referencePoints_s.end(); ++it)
  {
    ssc::Vector3D P_ref = ssc::CoordinateSystemHelpers::get_toMfrom(sensor, ref).coord(it->second);
    this->addPoint(P_ref);
  }
}

}//end namespace cx
