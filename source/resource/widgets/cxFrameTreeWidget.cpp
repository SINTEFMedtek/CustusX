/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxFrameTreeWidget.h"

#include <QVBoxLayout>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include "cxFrameForest.h"
#include "cxData.h"
#include "cxPatientModelService.h"

namespace cx
{

FrameTreeWidget::FrameTreeWidget(PatientModelServicePtr patientService, QWidget* parent) :
  BaseWidget(parent, "frame_tree_widget", "Frame Tree"),
  mPatientService(patientService)
{
  QVBoxLayout* layout = new QVBoxLayout(this);

  //layout->setMargin(0);
  this->setToolTip("A tree displaying relations between coordinate spaces");
  mTreeWidget = new QTreeWidget(this);
  layout->addWidget(mTreeWidget);
  mTreeWidget->setHeaderLabels(QStringList() << "Frame");

  // TODO this must also listen to all changed() in all data
  connect(mPatientService.get(), SIGNAL(dataAddedOrRemoved()), this, SLOT(dataLoadedSlot()));
}

void FrameTreeWidget::dataLoadedSlot()
{
  for (std::map<QString, DataPtr>::iterator iter=mConnectedData.begin(); iter!=mConnectedData.end(); ++iter)
  {
	disconnect(iter->second.get(), SIGNAL(transformChanged()), this, SLOT(setModified()));
  }

  mConnectedData = mPatientService->getDatas();

  for (std::map<QString, DataPtr>::iterator iter=mConnectedData.begin(); iter!=mConnectedData.end(); ++iter)
  {
	connect(iter->second.get(), SIGNAL(transformChanged()), this, SLOT(setModified()));
  }

  this->setModified();
}

void FrameTreeWidget::prePaintEvent()
{
	this->rebuild();
}

void FrameTreeWidget::rebuild()
{
  mTreeWidget->clear();

  FrameForest forest(mPatientService->getDatas());
  QDomElement root = forest.getDocument().documentElement();

  this->fill(mTreeWidget->invisibleRootItem(), root);

  mTreeWidget->expandToDepth(10);
  mTreeWidget->resizeColumnToContents(0);
}

void FrameTreeWidget::fill(QTreeWidgetItem* parent, QDomNode node)
{
  for (QDomNode child = node.firstChild(); !child.isNull(); child = child.nextSibling())
  {
    QString frameName = child.toElement().tagName();

    // if frame refers to a data, use its name instead.
	DataPtr data = mPatientService->getData(frameName);
    if (data)
      frameName = data->getName();

    QTreeWidgetItem* item = new QTreeWidgetItem(parent, QStringList() << frameName);
    this->fill(item, child);
  }
}

}
