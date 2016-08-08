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
#include "cxPointSamplingWidget.h"

#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QStringList>
#include <QVBoxLayout>
#include <QHeaderView>


#include "cxTypeConversions.h"
#include "cxCoordinateSystemHelpers.h"
#include "cxTrackingService.h"
#include "cxManualTool.h"
#include "cxLogger.h"
#include "cxLegacySingletons.h"
#include "cxSpaceProvider.h"
#include "cxPatientModelService.h"
#include "cxViewGroupData.h"
#include "cxViewService.h"


namespace cx
{

PointSamplingWidget::PointSamplingWidget(QWidget* parent) :
  BaseWidget(parent, "point_sampling_widget", "Point sampler/3D ruler"),
  mVerticalLayout(new QVBoxLayout(this)),
  mTable(new QTableWidget(this)),
  mActiveLandmark(""),
  mAddButton(new QPushButton("Add", this)),
  mEditButton(new QPushButton("Resample", this)),
  mRemoveButton(new QPushButton("Remove", this)),
  mLoadReferencePointsButton(new QPushButton("Load reference points", this))
{
	this->setToolTip("Sample and store points");
	connect(trackingService().get(), &TrackingService::stateChanged, this, &PointSamplingWidget::updateSlot);

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

  ViewGroupDataPtr data = viewService()->getGroup(0);
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
  mTable->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
  mTable->setSelectionBehavior(QAbstractItemView::SelectRows);

  for (unsigned i = 0; i < mSamples.size(); ++i)
  {
    std::vector<QTableWidgetItem*> items(3); // name, coordinates, delta

    Vector3D coord = mSamples[i].getCoord();

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
  mLoadReferencePointsButton->setEnabled(trackingService()->getReferenceTool() ? true : false);
}

void PointSamplingWidget::addPoint(Vector3D point)
{
  // find unique uid:
  int max = 0;
  for (unsigned i=0; i<mSamples.size(); ++i)
  {
    max = std::max(max, qstring_cast(mSamples[i].getUid()).toInt());
  }
  QString uid = qstring_cast(max+1);

  mSamples.push_back(Landmark(uid, point));
  mActiveLandmark = uid;

  this->updateSlot();
}

void PointSamplingWidget::setManualTool(const Vector3D& p_r)
{
  ToolPtr tool = trackingService()->getManualTool();

  //Transform3D sMr = mSliceProxy->get_sMr();
  Transform3D rMpr = patientService()->get_rMpr();
  Transform3D prMt = tool->get_prMt();

  // find tool position in r
  Vector3D tool_t(0,0,tool->getTooltipOffset());
  Vector3D tool_r = (rMpr*prMt).coord(tool_t);

  // find click position in s.
  //Vector3D click_s = get_vpMs().inv().coord(click_vp);

  // compute the new tool position in slice space as a synthesis of the plane part of click and the z part of original.
  //Vector3D cross_s(click_s[0], click_s[1], tool_s[2]);
  // compute the position change and transform to patient.
  Vector3D delta_r = p_r - tool_r;
  Vector3D delta_pr = rMpr.inv().vector(delta_r);

  // MD is the actual tool movement in patient space, matrix form
  Transform3D MD = createTransformTranslate(delta_pr);
  // set new tool position to old modified by MD:
  tool->set_prMt(MD*prMt);
}

void PointSamplingWidget::addButtonClickedSlot()
{
  this->addPoint(this->getSample());
}

Vector3D PointSamplingWidget::getSample() const
{
//  CoordinateSystem ref = spaceProvider()->getR();
  Vector3D P_ref = spaceProvider()->getActiveToolTipPoint(CoordinateSystem::reference(), true);

  return P_ref;
}

void PointSamplingWidget::editButtonClickedSlot()
{
  for (unsigned i=0; i<mSamples.size(); ++i)
  {
    if (mSamples[i].getUid()!=mActiveLandmark)
      continue;
    mSamples[i] = Landmark(mActiveLandmark, getSample());
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
  ToolPtr refTool = trackingService()->getReferenceTool();
  if(!refTool) // we only load reference points from reference tools
  {
	reportDebug("No reference tool, cannot load reference points into the pointsampler");
    return;
  }

  std::map<int, Vector3D> referencePoints_s = refTool->getReferencePoints();
  if(referencePoints_s.empty())
  {
    reportWarning("No referenceppoints in reference tool "+refTool->getName());
    return;
  }

  CoordinateSystem ref = spaceProvider()->getR();
  CoordinateSystem sensor = spaceProvider()->getS(refTool);

  std::map<int, Vector3D>::iterator it = referencePoints_s.begin();
  for(; it != referencePoints_s.end(); ++it)
  {
	Vector3D P_ref = spaceProvider()->get_toMfrom(sensor, ref).coord(it->second);
    this->addPoint(P_ref);
  }
}

}//end namespace cx
