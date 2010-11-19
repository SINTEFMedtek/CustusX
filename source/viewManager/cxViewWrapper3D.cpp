/*
 * cxViewWrapper3D.cpp
 *
 *  Created on: Mar 24, 2010
 *      Author: christiana
 */

#include "cxViewWrapper3D.h"
#include <vector>
#include <QSettings>
#include <QAction>
#include <QMenu>
#include <vtkTransform.h>
#include <vtkCamera.h>
#include <vtkAbstractVolumeMapper.h>
#include <vtkVolumeMapper.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>

#include "sscView.h"
#include "sscSliceProxy.h"
#include "sscSlicerRepSW.h"
#include "sscTool2DRep.h"
#include "sscOrientationAnnotationRep.h"
#include "sscDisplayTextRep.h"
#include "sscMessageManager.h"
#include "cxToolManager.h"
#include "sscSlicePlanes3DRep.h"
#include "cxLandmarkRep.h"
#include "cxRepManager.h"
#include "sscDataManager.h"
#include "sscMesh.h"
#include "sscProbeRep.h"
#include "sscGeometricRep.h"
#include "sscToolRep3D.h"
#include "sscVolumetricRep.h"
#include "sscTypeConversions.h"
#include "cxCameraControl.h"
#include "sscRealTimeStreamSource.h"
#include "RTSource/sscRT2DRep.h"

namespace cx
{


ssc::AxesRepPtr ToolAxisConnector::getAxis_t()
{
	return mAxis_t;
}

ssc::AxesRepPtr ToolAxisConnector::getAxis_s()
{
	return mAxis_s;
}

ToolAxisConnector::ToolAxisConnector(ssc::ToolPtr tool)
{
	mTool = tool;
	mAxis_s = ssc::AxesRep::New(tool->getUid()+"_axis_s");
	mAxis_t = ssc::AxesRep::New(tool->getUid()+"_axis_t");

	mAxis_t->setAxisLength(40);
	mAxis_t->setShowAxesLabels(false);
	//mAxis_t->setCaption(tool->getName()+"_t", ssc::Vector3D(1,1,0.7));
	mAxis_t->setCaption("t", ssc::Vector3D(0.7,1,0.7));
	mAxis_t->setFontSize(0.03);

	mAxis_s->setAxisLength(30);
	mAxis_s->setShowAxesLabels(false);
	mAxis_s->setCaption("s", ssc::Vector3D(1,1,0));
	mAxis_s->setFontSize(0.03);

	connect(mTool.get(), SIGNAL(toolTransformAndTimestamp(Transform3D, double)), this, SLOT(transformChangedSlot()));
	connect(mTool.get(), SIGNAL(toolVisible(bool)), this, SLOT(visibleSlot()));
	visibleSlot();
	transformChangedSlot();
}

void ToolAxisConnector::transformChangedSlot()
{
	ssc::Transform3D rMt = *ssc::toolManager()->get_rMpr() * mTool->get_prMt();
	ssc::Transform3D sMt = mTool->getCalibration_sMt();
	mAxis_t->setTransform(rMt);
	mAxis_s->setTransform(rMt*sMt.inv());
}

void ToolAxisConnector::visibleSlot()
{
	mAxis_t->setVisible(mTool->getVisible());
	mAxis_s->setVisible(mTool->getVisible());
	if (similar(mTool->getCalibration_sMt(), ssc::Transform3D()))
	{
		mAxis_s->setVisible(false);
	}
}

///--------------------------------------------------------
///--------------------------------------------------------

///--------------------------------------------------------
///--------------------------------------------------------



ViewWrapper3D::ViewWrapper3D(int startIndex, ssc::View* view)
{
  mShowAxes = false;
  mView = view;
  this->connectContextMenu(mView);
  QString index = QString::number(startIndex);

  //view->getRenderer()->GetActiveCamera()->SetParallelProjection(true);
  view->getRenderer()->GetActiveCamera()->SetParallelProjection(false);

  mLandmarkRep = repManager()->getLandmarkRep("LandmarkRep_"+index);
  mProbeRep = repManager()->getProbeRep("ProbeRep_"+index);

  // plane type text rep
  mPlaneTypeText = ssc::DisplayTextRep::New("planeTypeRep_"+mView->getName(), "");
  mPlaneTypeText->addText(ssc::Vector3D(0,1,0), "3D", ssc::Vector3D(0.98, 0.02, 0.0));
  mView->addRep(mPlaneTypeText);

  //data name text rep
  mDataNameText = ssc::DisplayTextRep::New("dataNameText_"+mView->getName(), "");
  mDataNameText->addText(ssc::Vector3D(0,1,0), "not initialized", ssc::Vector3D(0.02, 0.02, 0.0));
  mView->addRep(mDataNameText);

  connect(ssc::toolManager(), SIGNAL(initialized()), this, SLOT(toolsAvailableSlot()));
  connect(ssc::dataManager(), SIGNAL(activeImageChanged(const QString&)), this, SLOT(activeImageChangedSlot()));
  this->toolsAvailableSlot();
  //showAxesActionSlot(true);
}

ViewWrapper3D::~ViewWrapper3D()
{
  if (mView)
    mView->removeReps();
}

void ViewWrapper3D::appendToContextMenu(QMenu& contextMenu)
{
  QAction* slicePlanesAction = new QAction("Show Slice Planes", &contextMenu);
  slicePlanesAction->setCheckable(true);
  slicePlanesAction->setChecked(mSlicePlanes3DRep->getProxy()->getVisible());
  connect(slicePlanesAction, SIGNAL(triggered(bool)), this, SLOT(showSlicePlanesActionSlot(bool)));

  QAction* fillSlicePlanesAction = new QAction("Fill Slice Planes", &contextMenu);
  fillSlicePlanesAction->setCheckable(true);
  fillSlicePlanesAction->setEnabled(mSlicePlanes3DRep->getProxy()->getVisible());
  fillSlicePlanesAction->setChecked(mSlicePlanes3DRep->getProxy()->getDrawPlanes());
  connect(fillSlicePlanesAction, SIGNAL(triggered(bool)), this, SLOT(fillSlicePlanesActionSlot(bool)));

  QAction* resetCameraAction = new QAction("Reset Camera (r)", &contextMenu);
  //resetCameraAction->setCheckable(true);
  //resetCameraAction->setChecked(mSlicePlanes3DRep->getProxy()->getVisible());
  connect(resetCameraAction, SIGNAL(triggered()), this, SLOT(resetCameraActionSlot()));

  QAction* showAxesAction = new QAction("Show Coordinate Axes", &contextMenu);
  showAxesAction->setCheckable(true);
  showAxesAction->setChecked(mShowAxes);
  connect(showAxesAction, SIGNAL(triggered(bool)), this, SLOT(showAxesActionSlot(bool)));

  QAction* showManualTool = new QAction("Show Manual Tool", &contextMenu);
  showManualTool->setCheckable(true);
  showManualTool->setChecked(ToolManager::getInstance()->getManualTool()->getVisible());
  connect(showManualTool, SIGNAL(triggered(bool)), this, SLOT(showManualToolSlot(bool)));

  contextMenu.addSeparator();
  contextMenu.addAction(resetCameraAction);
  contextMenu.addAction(showAxesAction);
  contextMenu.addAction(showManualTool);
  contextMenu.addSeparator();
  contextMenu.addAction(slicePlanesAction);
  contextMenu.addAction(fillSlicePlanesAction);
}

void ViewWrapper3D::setViewGroup(ViewGroupDataPtr group)
{
  ViewWrapper::setViewGroup(group);

  connect(group.get(), SIGNAL(initialized()), this, SLOT(resetCameraActionSlot()));
  mView->getRenderer()->SetActiveCamera(mViewGroup->getCamera3D()->getCamera());
}

void ViewWrapper3D::showAxesActionSlot(bool checked)
{
	if (mShowAxes==checked)
		return;

	mShowAxes = checked;

	if (mShowAxes)
	{
		if (!mRefSpaceAxisRep)
		{
			  mRefSpaceAxisRep = ssc::AxesRep::New("refspace_axis");
			  mRefSpaceAxisRep->setCaption("ref", ssc::Vector3D(1,0,0));
			  mRefSpaceAxisRep->setFontSize(0.03);

			  mView->addRep(mRefSpaceAxisRep);
		}

//	  ssc::map<QString, ssc::AxesRepPtr> mDataSpaceAxisRep;
	  std::vector<ssc::DataPtr> data = mViewGroup->getData();
	  for (unsigned i=0; i<data.size(); ++i)
	  {
	    ssc::AxesRepPtr rep = ssc::AxesRep::New(data[i]->getName()+"_axis");
      rep->setCaption(data[i]->getName(), ssc::Vector3D(1,0,0));
      rep->setFontSize(0.03);
      rep->setTransform(data[i]->get_rMd());
      mDataSpaceAxisRep[data[i]->getUid()] = rep;
      mView->addRep(rep);
	  }


		ssc::ToolManager::ToolMapPtr tools = ssc::toolManager()->getTools();
		ssc::ToolManager::ToolMapPtr::value_type::iterator iter;
		for (iter=tools->begin(); iter!=tools->end(); ++iter)
		{
			mToolAxis[iter->first].reset(new ToolAxisConnector(iter->second));
			mToolAxis[iter->first].reset(new ToolAxisConnector(iter->second));
			mView->addRep(mToolAxis[iter->first]->getAxis_t());
			mView->addRep(mToolAxis[iter->first]->getAxis_s());
		}
	}
	else
	{
		mView->removeRep(mRefSpaceAxisRep);
		mRefSpaceAxisRep.reset();

    for (std::map<QString, ssc::AxesRepPtr>::iterator iter=mDataSpaceAxisRep.begin(); iter!=mDataSpaceAxisRep.end(); ++iter)
    {
      mView->removeRep(iter->second);
    }
    mDataSpaceAxisRep.clear();

    std::map<QString, ToolAxisConnectorPtr>::iterator iter;
		for (iter=mToolAxis.begin(); iter!=mToolAxis.end(); ++iter)
		{
			mView->removeRep(iter->second->getAxis_t());
			mView->removeRep(iter->second->getAxis_s());
		}
		mToolAxis.clear();
	}
}

void ViewWrapper3D::showManualToolSlot(bool visible)
{
  ToolManager::getInstance()->getManualTool()->setVisible(visible);
}

void ViewWrapper3D::resetCameraActionSlot()
{
  mView->getRenderer()->ResetCamera();
}

void ViewWrapper3D::showSlicePlanesActionSlot(bool checked)
{
  mSlicePlanes3DRep->getProxy()->setVisible(checked);
}
void ViewWrapper3D::fillSlicePlanesActionSlot(bool checked)
{
  mSlicePlanes3DRep->getProxy()->setDrawPlanes(checked);
}

void ViewWrapper3D::imageAdded(ssc::ImagePtr image)
{
  if (!mVolumetricReps.count(image->getUid()))
  {
    ssc::VolumetricRepPtr rep = repManager()->getVolumetricRep(image);

    mVolumetricReps[image->getUid()] = rep;
    mView->addRep(rep);
  }

//  mProbeRep->setImage(image);
//  mLandmarkRep->setImage(image);
  this->activeImageChangedSlot();

  updateView();

  //mView->getRenderer()->ResetCamera();
}

void ViewWrapper3D::updateView()
{
  std::vector<ssc::ImagePtr> images = mViewGroup->getImages();

  //update data name text rep
  QStringList text;
  for (unsigned i = 0; i < images.size(); ++i)
  {
    text << qstring_cast(images[i]->getName());
  }
  mDataNameText->setText(0, text.join("\n"));
}

void ViewWrapper3D::imageRemoved(const QString& uid)
{
  if (!mVolumetricReps.count(uid))
    return;

  ssc::messageManager()->sendDebug("Remove image from view group 3d: "+uid);
  mView->removeRep(mVolumetricReps[uid]);
  mVolumetricReps.erase(uid);

  this->activeImageChangedSlot();
//  if (mProbeRep->getImage() && mProbeRep->getImage()->getUid()==suid)
//    mProbeRep->setImage(ssc::ImagePtr());
//  if (mLandmarkRep->getImage() && mLandmarkRep->getImage()->getUid()==suid)
//    mLandmarkRep->setImage(ssc::ImagePtr());

  this->updateView();
}

void ViewWrapper3D::activeImageChangedSlot()
{
  ssc::ImagePtr image = ssc::dataManager()->getActiveImage();

  // only show landmarks belonging to image visible in this view:
  std::vector<ssc::ImagePtr> images = mViewGroup->getImages();
  if (!std::count(images.begin(), images.end(), image))
    image.reset();

  mProbeRep->setImage(image);
  mLandmarkRep->setImage(image);
}

void ViewWrapper3D::meshAdded(ssc::MeshPtr data)
{
  ssc::GeometricRepPtr rep = ssc::GeometricRep::New(data->getUid()+"_geom_rep");
  rep->setMesh(data);
  mGeometricReps[data->getUid()] = rep;
  mView->addRep(rep);
  this->updateView();

//  mView->getRenderer()->ResetCamera();
}

void ViewWrapper3D::meshRemoved(const QString& uid)
{
  if (!mGeometricReps.count(uid))
    return;

  mView->removeRep(mGeometricReps[uid]);
  mGeometricReps.erase(uid);
  this->updateView();
}
  
ssc::View* ViewWrapper3D::getView()
{
  return mView;
}

void ViewWrapper3D::dominantToolChangedSlot()
{
  ssc::ToolPtr dominantTool = ssc::toolManager()->getDominantTool();
  mProbeRep->setTool(dominantTool);
}


void ViewWrapper3D::toolsAvailableSlot()
{
 // std::cout <<"void ViewWrapper3D::toolsAvailableSlot() " << std::endl;
  // we want to do this also when nonconfigured and manual tool is present
//  if (!toolManager()->isConfigured())
//    return;

  ssc::ToolManager::ToolMapPtr tools = ssc::toolManager()->getTools();
  ssc::ToolManager::ToolMapPtr::value_type::iterator iter;
  for (iter=tools->begin(); iter!=tools->end(); ++iter)
  {
    ssc::ToolPtr tool = iter->second;
    //experiment with showing toolrep for reference tools
//    if(tool->getType() == ssc::Tool::TOOL_REFERENCE)
//      continue;

    QString uid = tool->getUid()+"_rep3d_"+this->mView->getUid();
    if (!mToolReps.count(uid))
    {
      mToolReps[uid] = repManager()->getDynamicToolRep3DRep(uid);
    }
    ssc::ToolRep3DPtr toolRep = mToolReps[uid];
//    std::cout << "setting 3D tool rep for " << iter->second->getName() << std::endl;
    toolRep->setTool(tool);
    toolRep->setOffsetPointVisibleAtZeroOffset(true);
    mView->addRep(toolRep);
   // ssc::messageManager()->sendDebug("ToolRep3D for tool "+tool->getName()+" added to view "+mView->getName()+".");

    if (!mRTStreamRep)
    {
      std::cout << "getting stream source: " << ssc::dataManager()->getStream("us_openigtlink_source") << std::endl;
      mRTStreamRep.reset(new ssc::RealTimeStream2DRep("rtrep", "rtrep"));
    //  ssc::RealTimeStream2DRepPtr rtRep(new ssc::RealTimeStream2DRep("rtrep", "rtrep"));
      mRTStreamRep->setRealtimeStream(ssc::dataManager()->getStream("us_openigtlink_source"));
      mRTStreamRep->setTool(tool);
      mView->addRep(mRTStreamRep);
    }
  }


}


void ViewWrapper3D::setRegistrationMode(ssc::REGISTRATION_STATUS mode)
{
  if (mode==ssc::rsNOT_REGISTRATED)
  {
    mView->removeRep(mLandmarkRep);
    mView->removeRep(mProbeRep);
    
    disconnect(ssc::toolManager(), SIGNAL(dominantToolChanged(const QString&)), this, SLOT(dominantToolChangedSlot()));
  }
  if (mode==ssc::rsIMAGE_REGISTRATED)
  {
    mView->addRep(mLandmarkRep);
    mView->addRep(mProbeRep);

    connect(ssc::toolManager(), SIGNAL(dominantToolChanged(const QString&)), this, SLOT(dominantToolChangedSlot()));
    this->dominantToolChangedSlot();
  }
  if (mode==ssc::rsPATIENT_REGISTRATED)
  {
    mView->addRep(mLandmarkRep);
  }
}

void ViewWrapper3D::setSlicePlanesProxy(ssc::SlicePlanesProxyPtr proxy)
{
  mSlicePlanes3DRep = ssc::SlicePlanes3DRep::New("uid");
  mSlicePlanes3DRep->setProxy(proxy);
  mView->addRep(mSlicePlanes3DRep);
}


//------------------------------------------------------------------------------


}
