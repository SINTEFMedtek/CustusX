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
