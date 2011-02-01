/*
 * cxViewWrapperRTStream.cpp
 *
 *  Created on: Mar 24, 2010
 *      Author: christiana
 */

#include "cxViewWrapperRTStream.h"
#include <vector>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include "sscUtilHelpers.h"
#include "sscView.h"
#include "sscSliceProxy.h"
#include "sscSlicerRepSW.h"
#include "sscTool2DRep.h"
#include "sscOrientationAnnotationRep.h"
#include "sscDisplayTextRep.h"
#include "sscMessageManager.h"
#include "cxRepManager.h"
#include "cxDataLocations.h"
#include "sscDataManager.h"
#include "cxViewManager.h"
#include "cxToolManager.h"
#include "cxViewGroup.h"
#include "sscDefinitionStrings.h"
#include "sscSlicePlanes3DRep.h"
#include "sscDefinitionStrings.h"
#include "sscSliceComputer.h"
#include "sscGeometricRep2D.h"
#include "sscRTStreamRep.h"

namespace cx
{

ViewWrapperRTStream::ViewWrapperRTStream(ssc::View* view)
{
  mView = view;
  this->connectContextMenu(mView);

  // disable vtk interactor: this wrapper IS an interactor
  mView->getRenderWindow()->GetInteractor()->Disable();
  mView->getRenderer()->GetActiveCamera()->SetParallelProjection(true);
  double clipDepth = 1.0; // 1mm depth, i.e. all 3D props rendered outside this range is not shown.
  mView->getRenderer()->GetActiveCamera()->SetClippingRange(-clipDepth/2.0, clipDepth/2.0);

  connect(ssc::dataManager(), SIGNAL(streamLoaded()), this, SLOT(streamLoadedSlot()));
  connect(ssc::toolManager(), SIGNAL(dominantToolChanged(const QString&)), this, SLOT(dominantToolChangedSlot()));

  addReps();
  this->dominantToolChangedSlot();
}

ViewWrapperRTStream::~ViewWrapperRTStream()
{
  if (mView)
    mView->removeReps();
}

ssc::View* ViewWrapperRTStream::getView()
{
  return mView;
}

void ViewWrapperRTStream::appendToContextMenu(QMenu& contextMenu)
{
  QAction* showSectorAction = new QAction("Show Sector", &contextMenu);
  showSectorAction->setCheckable(true);
  showSectorAction->setChecked(mStreamRep->getShowSector());
  connect(showSectorAction, SIGNAL(triggered(bool)), this, SLOT(showSectorActionSlot(bool)));

  contextMenu.addSeparator();
  contextMenu.addAction(showSectorAction);
}

void ViewWrapperRTStream::showSectorActionSlot(bool checked)
{
  mStreamRep->setShowSector(checked);
  DataLocations::getSettings()->setValue("showSectorInRTView", checked);
}

void ViewWrapperRTStream::streamLoadedSlot()
{
//  std::cout << "attempt add stream to rt view" << std::endl;
  if (ssc::dataManager()->getStreams().empty())
    return;

  mSource = ssc::dataManager()->getStreams().begin()->second;
  connect(mSource.get(), SIGNAL(newFrame()), this, SLOT(updateSlot()));

  mStreamRep.reset(new ssc::RealTimeStreamFixedPlaneRep("rtrep", "rtrep"));
  mStreamRep->setRealtimeStream(mSource);
//  rtRep->setTool(ssc::toolManager()->getDominantTool());
  mView->addRep(mStreamRep);
  mDataNameText->setText(0, "initialized");

  mStreamRep->setShowSector(DataLocations::getSettings()->value("showSectorInRTView").toBool());

//  std::cout << "added stream to rt view" << std::endl;
}

void ViewWrapperRTStream::dominantToolChangedSlot()
{
  mStreamRep->setTool(ssc::toolManager()->getDominantTool());
}

void ViewWrapperRTStream::updateSlot()
{
  if (!mSource)
    return;
  mDataNameText->setText(0, mSource->getName());
}

void ViewWrapperRTStream::addReps()
{
  // plane type text rep
  mPlaneTypeText = ssc::DisplayTextRep::New("planeTypeRep_"+mView->getName(), "");
  mPlaneTypeText->addText(ssc::Vector3D(0,1,0), "RT", ssc::Vector3D(0.98, 0.02, 0.0));
  mView->addRep(mPlaneTypeText);

  //data name text rep
  mDataNameText = ssc::DisplayTextRep::New("dataNameText_"+mView->getName(), "");
  mDataNameText->addText(ssc::Vector3D(0,1,0), "not initialized", ssc::Vector3D(0.02, 0.02, 0.0));
  mView->addRep(mDataNameText);

  this->streamLoadedSlot();
}


//void ViewWrapperRTStream::showSlot()
//{
////  dominantToolChangedSlot();
////  viewportChanged();
//}



//------------------------------------------------------------------------------
}
