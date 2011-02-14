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
#include "cxStateMachineManager.h"

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

  connect(ssc::dataManager(), SIGNAL(streamLoaded()), this, SLOT(configureSlot()));
  connect(ssc::toolManager(), SIGNAL(configured()), this, SLOT(configureSlot()));

  addReps();

  this->configureSlot();
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
  if (mStreamRep)
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

/** Setup connections to stream. Called when a stream is loaded into the datamanager, or if a probe is initialized
 *
 */
void ViewWrapperRTStream::configureSlot()
{
  std::cout << "!!!!! ViewWrapperRTStream::configureSlot " << ssc::dataManager()->getStreams().empty() << " "<< mTool.get() << std::endl;

  // if datamanager stream: connect it to rep
  if (!ssc::dataManager()->getStreams().empty() && !mTool)
  {
    this->setupRep(ssc::dataManager()->getStreams().begin()->second, ssc::ToolPtr());
  }

  // if probe tool exist, connect to probeChanged()
  if (mTool)
    disconnect(mTool->getProbe().get(), SIGNAL(sectorChanged()), this, SLOT(probeChangedSlot()));
  mTool = this->getProbe();
  if (mTool)
    connect(mTool->getProbe().get(), SIGNAL(sectorChanged()), this, SLOT(probeChangedSlot()));

  this->probeChangedSlot();
}

void ViewWrapperRTStream::probeChangedSlot()
{
  if (!mTool)
    return;
  std::cout << "!!!!! ViewWrapperRTStream::probeChangedSlot" << std::endl;

  // if probe has a stream, connect stream and probe to rep.
  this->setupRep(mTool->getProbe()->getRealTimeStreamSource(), mTool);
}

void ViewWrapperRTStream::setupRep(ssc::RealTimeStreamSourcePtr source, ssc::ToolPtr tool)
{
  std::cout << "setup rt view rep" << std::endl;

  if (mSource)
  {
    disconnect(mSource.get(), SIGNAL(newFrame()), this, SLOT(updateSlot()));
  }
  mSource = source;
  if (mSource)
  {
    connect(mSource.get(), SIGNAL(newFrame()), this, SLOT(updateSlot()));
  }

//  std::cout << "ViewWrapperRTStream::mSource " << mSource << std::endl;
  if (!mSource)
    return;
//  connect(mSource.get(), SIGNAL(newFrame()), this, SLOT(updateSlot()));

  mStreamRep.reset(new ssc::RealTimeStreamFixedPlaneRep("rtrep", "rtrep"));
  mStreamRep->setRealtimeStream(mSource);
  mStreamRep->setTool(tool);
  mView->addRep(mStreamRep);
  mDataNameText->setText(0, "initialized");
  mStreamRep->setShowSector(DataLocations::getSettings()->value("showSectorInRTView").toBool());

  ssc::messageManager()->sendInfo("Setup rt rep with source="+source->getName()+" and tool="+(tool?tool->getName():"none"));
//  std::cout << "completed rt view setup" << std::endl;

}

//void ViewWrapperRTStream::streamLoadedSlot()
//{
//  this->loadStream();
//
////  ssc::ToolPtr probe;
////  ssc::ToolManager::ToolMapPtr tools = ssc::toolManager()->getTools();
////  for (ssc::ToolManager::ToolMap::iterator iter=tools->begin(); iter!=tools->end(); ++iter)
////  {
////    if (iter->second->getProbe() && iter->second->getProbe()->isValid())
////    {
////      probe = iter->second;;
////      break;
////    }
////  }
////
////  if (!probe)
////    return;
////
////  std::cout << "ViewWrapperRTStream::streamLoadedSlot() " << probe->getName() << std::endl;
////
////  this->loadStream(probe);
////
//////  std::cout << "attempt add stream to rt view" << std::endl;
////  if (ssc::dataManager()->getStreams().empty())
////    return;
////
////  mSource = ssc::dataManager()->getStreams().begin()->second;
////  connect(mSource.get(), SIGNAL(newFrame()), this, SLOT(updateSlot()));
////
////  mStreamRep.reset(new ssc::RealTimeStreamFixedPlaneRep("rtrep", "rtrep"));
////  mStreamRep->setRealtimeStream(mSource);
//////  rtRep->setTool(ssc::toolManager()->getDominantTool());
////  mView->addRep(mStreamRep);
////  mDataNameText->setText(0, "initialized");
////
////  mStreamRep->setShowSector(DataLocations::getSettings()->value("showSectorInRTView").toBool());
////
//////  std::cout << "added stream to rt view" << std::endl;
//}

//void ViewWrapperRTStream::loadStream()
//{
//  std::cout << "attempt add stream to rt view" << std::endl;
//
//  if (ssc::dataManager()->getStreams().empty())
//    return;
//
//  if (mSource)
//  {
//    disconnect(mSource.get(), SIGNAL(newFrame()), this, SLOT(updateSlot()));
//    disconnect(mSource.get(), SIGNAL(connected()), this, SLOT(connectSlot()));
//  }
//  mSource = ssc::dataManager()->getStreams().begin()->second;
//  if (mSource)
//  {
//    connect(mSource.get(), SIGNAL(newFrame()), this, SLOT(updateSlot()));
//    connect(mSource.get(), SIGNAL(connected()), this, SLOT(connectSlot()));
//  }
//
////  mSource = stateManager()->getIGTLinkConnection()->getRTSource();
//
//  std::cout << "ViewWrapperRTStream::mSource " << mSource << std::endl;
//  if (!mSource)
//    return;
////  connect(mSource.get(), SIGNAL(newFrame()), this, SLOT(updateSlot()));
//
//  mStreamRep.reset(new ssc::RealTimeStreamFixedPlaneRep("rtrep", "rtrep"));
//  mStreamRep->setRealtimeStream(mSource);
////  mStreamRep->setTool(stateManager()->getIGTLinkConnection()->getStreamingProbe());
////  rtRep->setTool(ssc::toolManager()->getDominantTool());
//  mView->addRep(mStreamRep);
//  mDataNameText->setText(0, "initialized");
//
//  mStreamRep->setShowSector(DataLocations::getSettings()->value("showSectorInRTView").toBool());
//
//  std::cout << "added stream to rt view" << std::endl;
//}

//void ViewWrapperRTStream::connectSlot()
//{
//  ssc::ToolManager::ToolMapPtr tools = ssc::toolManager()->getTools();
//
//  for (ssc::ToolManager::ToolMap::iterator iter=tools->begin(); iter!=tools->end(); ++iter)
//  {
//    if (!iter->second->getProbe() || !iter->second->getProbe()->isValid())
//      continue;
//    mStreamRep->setTool(iter->second);
//  }
//
//}

ssc::ToolPtr ViewWrapperRTStream::getProbe()
{
  ssc::ToolManager::ToolMapPtr tools = ssc::toolManager()->getTools();

  for (ssc::ToolManager::ToolMap::iterator iter=tools->begin(); iter!=tools->end(); ++iter)
  {
//    std::cout << "!!!!! ViewWrapperRTStream::getProbe checking " << iter->first << std::endl;
    if (!iter->second->getProbe() || !iter->second->getProbe()->isValid())
      continue;
//    std::cout << "!!!!! ViewWrapperRTStream::getProbe " << iter->first << std::endl;

    return iter->second;
  }

  return ssc::ToolPtr();
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
}



//------------------------------------------------------------------------------
}
