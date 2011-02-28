#include "cxViewWrapper3D.h"

#include <vector>

#include "boost/bind.hpp"
#include "boost/function.hpp"

#include <QSettings>
#include <QAction>
#include <QMenu>

#include <vtkTransform.h>
#include <vtkCamera.h>
#include <vtkAbstractVolumeMapper.h>
#include <vtkVolumeMapper.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkInteractorObserver.h>

#include "cxDataLocations.h"
#include "sscView.h"
#include "sscSliceProxy.h"
#include "sscSlicerRepSW.h"
#include "sscTool2DRep.h"
#include "sscOrientationAnnotationRep.h"
#include "sscDisplayTextRep.h"
#include "sscMessageManager.h"
#include "cxToolManager.h"
#include "sscSlicePlanes3DRep.h"
#include "cxRepManager.h"
#include "sscDataManager.h"
#include "sscMesh.h"
#include "sscProbeRep.h"
#include "sscGeometricRep.h"
#include "sscToolRep3D.h"
#include "sscVolumetricRep.h"
#include "sscTypeConversions.h"
#include "cxCameraControl.h"
#include "sscRTSource.h"
#include "sscRTStreamRep.h"
#include "sscToolTracer.h"


namespace cx
{

class InteractionCallback : public vtkCommand
{
  typedef boost::function<void ()> CallbackType;
public:
  InteractionCallback() {}
  static InteractionCallback* New() {return new InteractionCallback;}
  void setCallback(CallbackType f)
  {
   mCallback = f;
  }
  virtual void Execute(vtkObject* caller, unsigned long, void*)
  {
    mCallback();
  }

private:
  CallbackType mCallback;
};


// --------------------------------------------------------
// --------------------------------------------------------
// --------------------------------------------------------


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
//  connect(ssc::dataManager(), SIGNAL(centerChanged()), this, SLOT(centerChangedSlot()));

  view->getRenderer()->GetActiveCamera()->SetParallelProjection(false);

  mImageLandmarkRep = ImageLandmarkRep::New("ImageLandmarkRep_"+index);
  mPatientLandmarkRep = PatientLandmarkRep::New("PatientLandmarkRep_"+index);
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

  mInteractorCallback = new InteractionCallback;
  mInteractorCallback->setCallback(boost::bind(&ViewWrapper3D::viewChanged, this));
  mView->getRenderWindow()->GetInteractor()->GetInteractorStyle()->AddObserver(vtkCommand::InteractionEvent, mInteractorCallback);
  mView->getRenderWindow()->GetInteractor()->GetInteractorStyle()->AddObserver(vtkCommand::EndInteractionEvent, mInteractorCallback);
}

///**Change the camera focal point to the datamanager::center.
// * Keep pos of camera constant relative to the focus.
// */
//void ViewWrapper3D::centerChangedSlot()
//{
//  ssc::Vector3D c = ssc::DataManager::getInstance()->getCenter();
//  vtkCameraPtr camera = mViewGroup->getCamera3D()->getCamera();
//  ssc::Vector3D f(camera->GetFocalPoint());
//  ssc::Vector3D p(camera->GetPosition());
//  ssc::Vector3D delta = c-f;
//  f += delta;
//  p += delta;
//  camera->SetFocalPoint(f.begin());
//  camera->SetPosition(p.begin());
//}

ViewWrapper3D::~ViewWrapper3D()
{
  if (mView)
    mView->removeReps();

  mView->getRenderWindow()->GetInteractor()->GetInteractorStyle()->RemoveObserver(mInteractorCallback);
  mView->getRenderWindow()->GetInteractor()->GetInteractorStyle()->RemoveObserver(mInteractorCallback);
}

void ViewWrapper3D::viewChanged()
{
  if (!mView || !mView->getRenderer())
    return;
  if (mView->getRenderer()->GetSize()[0]==0)
    return;

  ssc::DoubleBoundingBox3D vp_w(-1,1,-1,1,-1,1);
  mView->getRenderer()->ViewToWorld(vp_w[0], vp_w[2], vp_w[4]);
  mView->getRenderer()->ViewToWorld(vp_w[1], vp_w[3], vp_w[5]);
//  std::cout << "    vp_w " << vp_w << std::endl;
//  std::cout << this << " dim " << vp_w.range() << std::endl;
  double size = (vp_w.range()[0] + vp_w.range()[1]) / 2;

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
  connect(resetCameraAction, SIGNAL(triggered()), this, SLOT(resetCameraActionSlot()));

  QAction* centerImageAction = new QAction("Center to image", &contextMenu);
  connect(centerImageAction, SIGNAL(triggered()), this, SLOT(centerImageActionSlot()));

  QAction* centerToolAction = new QAction("Center to tool", &contextMenu);
  connect(centerToolAction, SIGNAL(triggered()), this, SLOT(centerToolActionSlot()));

  QAction* showAxesAction = new QAction("Show Coordinate Axes", &contextMenu);
  showAxesAction->setCheckable(true);
  showAxesAction->setChecked(mShowAxes);
  connect(showAxesAction, SIGNAL(triggered(bool)), this, SLOT(showAxesActionSlot(bool)));

  QAction* showManualTool = new QAction("Show Manual Tool", &contextMenu);
  showManualTool->setCheckable(true);
  showManualTool->setChecked(ToolManager::getInstance()->getManualTool()->getVisible());
  connect(showManualTool, SIGNAL(triggered(bool)), this, SLOT(showManualToolSlot(bool)));

  QAction* showToolPath = new QAction("Show Tool Path", &contextMenu);
  showToolPath->setCheckable(true);
  ssc::ToolRep3DPtr activeRep3D = repManager()->findFirstRep<ssc::ToolRep3D>(mView->getReps(), ssc::toolManager()->getDominantTool());
  showToolPath->setChecked(activeRep3D->getTracer()->isRunning());
  connect(showToolPath, SIGNAL(triggered(bool)), this, SLOT(showToolPathSlot(bool)));


  QAction* showRefTool = new QAction("Show Reference Tool", &contextMenu);
  showRefTool->setDisabled(true);
  showRefTool->setCheckable(true);
  ssc::ToolPtr refTool = ToolManager::getInstance()->getReferenceTool();
  if(refTool)
  {
    showRefTool->setText("Show "+refTool->getName());
    showRefTool->setEnabled(true);
    showRefTool->setChecked(repManager()->findFirstRep<ssc::ToolRep3D>(mView->getReps(), refTool));
    connect(showRefTool, SIGNAL(toggled(bool)), this, SLOT(showRefToolSlot(bool)));
  }

  contextMenu.addSeparator();
  contextMenu.addAction(resetCameraAction);
  contextMenu.addAction(centerImageAction);
  contextMenu.addAction(centerToolAction);
  contextMenu.addAction(showAxesAction);
  contextMenu.addSeparator();
  contextMenu.addAction(showManualTool);
  contextMenu.addAction(showRefTool);
  contextMenu.addAction(showToolPath);
  contextMenu.addSeparator();
  contextMenu.addAction(slicePlanesAction);
  contextMenu.addAction(fillSlicePlanesAction);
}

void ViewWrapper3D::setViewGroup(ViewGroupDataPtr group)
{
  ViewWrapper::setViewGroup(group);

  connect(group.get(), SIGNAL(initialized()), this, SLOT(resetCameraActionSlot()));
  connect(group.get(), SIGNAL(optionsChanged()), this, SLOT(optionChangedSlot()));
  mView->getRenderer()->SetActiveCamera(mViewGroup->getCamera3D()->getCamera());
}

void ViewWrapper3D::showToolPathSlot(bool checked)
{
  ssc::ToolRep3DPtr activeRep3D = repManager()->findFirstRep<ssc::ToolRep3D>(mView->getReps(), ssc::toolManager()->getDominantTool());
  if (activeRep3D->getTracer()->isRunning())
  {
    activeRep3D->getTracer()->stop();
    activeRep3D->getTracer()->clear();
  }
  else
  {
    activeRep3D->getTracer()->start();
  }

  DataLocations::getSettings()->setValue("showToolPath", checked);
//  showToolPath->setChecked(DataLocations::getSettings()->value("showToolPath"));
//  ssc::toolManager()->getDominantTool()->setShowPath(checked);
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

void ViewWrapper3D::centerImageActionSlot()
{
  Navigation().centerToImage(ssc::dataManager()->getActiveImage());
}
void ViewWrapper3D::centerToolActionSlot()
{
  Navigation().centerToTooltip();
}

void ViewWrapper3D::showSlicePlanesActionSlot(bool checked)
{
  mSlicePlanes3DRep->getProxy()->setVisible(checked);
  DataLocations::getSettings()->setValue("showSlicePlanes", checked);
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

  this->activeImageChangedSlot();

  updateView();

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

  //ssc::messageManager()->sendDebug("Remove image from view group 3d: "+uid);
  mView->removeRep(mVolumetricReps[uid]);
  mVolumetricReps.erase(uid);

  this->activeImageChangedSlot();

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
  mImageLandmarkRep->setImage(image);
}

void ViewWrapper3D::showRefToolSlot(bool checked)
{
  ssc::ToolPtr refTool = ssc::toolManager()->getReferenceTool();
  if(!refTool)
    return;
  ssc::ToolRep3DPtr refRep = repManager()->findFirstRep<ssc::ToolRep3D>(mView->getReps(), refTool);
  if(!refRep)
  {
    refRep = ssc::ToolRep3D::New(refTool->getUid()+"_rep3d_"+this->mView->getUid());
    refRep->setTool(refTool);
  }

  if(checked) //should show
    mView->addRep(refRep);
  else//should not show
    mView->removeRep(refRep);
}

void ViewWrapper3D::meshAdded(ssc::MeshPtr data)
{
  ssc::GeometricRepPtr rep = ssc::GeometricRep::New(data->getUid()+"_geom_rep");
  rep->setMesh(data);
  mGeometricReps[data->getUid()] = rep;
  mView->addRep(rep);
  this->updateView();

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
  ssc::ToolManager::ToolMapPtr tools = ssc::toolManager()->getTools();
  ssc::ToolManager::ToolMapPtr::value_type::iterator iter;
  for (iter=tools->begin(); iter!=tools->end(); ++iter)
  {
    ssc::ToolPtr tool = iter->second;
    if(tool->getType() == ssc::Tool::TOOL_REFERENCE)
      continue;

    ssc::ToolRep3DPtr toolRep = repManager()->findFirstRep<ssc::ToolRep3D>(mView->getReps(), tool);
    if(!toolRep)
    {
      toolRep = ssc::ToolRep3D::New(tool->getUid()+"_rep3d_"+this->mView->getUid());
      if (DataLocations::getSettings()->value("showToolPath").toBool())
        toolRep->getTracer()->start();
    }

//    QString uid = tool->getUid()+"_rep3d_"+this->mView->getUid();
//    if (!mToolReps.count(uid))
//    {
//      mToolReps[uid] = repManager()->getDynamicToolRep3DRep(uid);
//    }
//    ssc::ToolRep3DPtr toolRep = mToolReps[uid];

//    std::cout << "setting 3D tool rep for " << iter->second->getName() << std::endl;
    toolRep->setTool(tool);
    toolRep->setOffsetPointVisibleAtZeroOffset(true);
    mView->addRep(toolRep);
   // ssc::messageManager()->sendDebug("ToolRep3D for tool "+tool->getName()+" added to view "+mView->getName()+".");

//    flytt denne koden inn i tool3d. flytt rtrep inn i ssc
//    if (!mRTStreamRep)
//    {
//      std::cout << "getting stream source: " << ssc::dataManager()->getStream("us_openigtlink_source") << std::endl;
//      mRTStreamRep.reset(new ssc::RealTimeStreamRep("rtrep", "rtrep"));
//    //  ssc::RealTimeStream2DRepPtr rtRep(new ssc::RealTimeStream2DRep("rtrep", "rtrep"));
//      mRTStreamRep->setTool(tool);
//      mRTStreamRep->setRealtimeStream(ssc::dataManager()->getStream("us_openigtlink_source"));
//      mView->addRep(mRTStreamRep);
//    }
  }
}

void ViewWrapper3D::optionChangedSlot()
{
  ViewGroupData::Options options = mViewGroup->getOptions();

  this->showLandmarks(options.mShowLandmarks);
  this->showPointPickerProbe(options.mShowPointPickerProbe);
}

void ViewWrapper3D::showLandmarks(bool on)
{
  if (mImageLandmarkRep->isConnectedToView(mView) == on)
    return;

  if (on)
  {
    mView->addRep(mPatientLandmarkRep);
    mView->addRep(mImageLandmarkRep);
  }
  else
  {
    mView->removeRep(mPatientLandmarkRep);
    mView->removeRep(mImageLandmarkRep);
  }
}

void ViewWrapper3D::showPointPickerProbe(bool on)
{
  if (mProbeRep->isConnectedToView(mView) == on)
    return;

  if (on)
  {
    mView->addRep(mProbeRep);
    connect(ssc::toolManager(), SIGNAL(dominantToolChanged(const QString&)), this, SLOT(dominantToolChangedSlot()));
    this->dominantToolChangedSlot();
  }
  else
  {
    mView->removeRep(mProbeRep);
    disconnect(ssc::toolManager(), SIGNAL(dominantToolChanged(const QString&)), this, SLOT(dominantToolChangedSlot()));
  }
}


//void ViewWrapper3D::setRegistrationMode(ssc::REGISTRATION_STATUS mode)
//{
//  if (mode==ssc::rsNOT_REGISTRATED)
//  {
//    mView->removeRep(mPatientLandmarkRep);
//    mView->removeRep(mImageLandmarkRep);
//    mView->removeRep(mProbeRep);
//
//    disconnect(ssc::toolManager(), SIGNAL(dominantToolChanged(const QString&)), this, SLOT(dominantToolChangedSlot()));
//  }
//  if (mode==ssc::rsIMAGE_REGISTRATED)
//  {
//    mView->addRep(mPatientLandmarkRep);
//    mView->addRep(mImageLandmarkRep);
//    mView->addRep(mProbeRep);
//
//    connect(ssc::toolManager(), SIGNAL(dominantToolChanged(const QString&)), this, SLOT(dominantToolChangedSlot()));
//    this->dominantToolChangedSlot();
//  }
//  if (mode==ssc::rsPATIENT_REGISTRATED)
//  {
//    mView->addRep(mPatientLandmarkRep);
//    mView->addRep(mImageLandmarkRep);
//  }
//}

void ViewWrapper3D::setSlicePlanesProxy(ssc::SlicePlanesProxyPtr proxy)
{
  mSlicePlanes3DRep = ssc::SlicePlanes3DRep::New("uid");
  mSlicePlanes3DRep->setProxy(proxy);
  bool show = DataLocations::getSettings()->value("showSlicePlanes").toBool();
  mSlicePlanes3DRep->getProxy()->setVisible(show); // init with default value

  mView->addRep(mSlicePlanes3DRep);
}
//------------------------------------------------------------------------------
}
