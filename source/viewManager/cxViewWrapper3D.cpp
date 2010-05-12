/*
 * cxViewWrapper3D.cpp
 *
 *  Created on: Mar 24, 2010
 *      Author: christiana
 */

#include "cxViewWrapper3D.h"
#include <vector>
#include <QSettings>
#include <vtkRenderWindow.h>

#include "sscView.h"
#include "sscTypeConversions.h"
#include "sscSliceProxy.h"
#include "sscSlicerRepSW.h"
#include "sscTool2DRep.h"
#include "sscOrientationAnnotationRep.h"
#include "sscDisplayTextRep.h"
#include "cxRepManager.h"
#include "cxDataManager.h"
#include "cxToolManager.h"
#include "cxMessageManager.h"
#include "cxInriaRep2D.h"
#include "cxLandmarkRep.h"

namespace cx
{

ViewWrapper3D::ViewWrapper3D(int startIndex, ssc::View* view)
{
  mView = view;
  this->connectContextMenu(mView);
  std::string index = QString::number(startIndex).toStdString();

  mVolumetricRep = repManager()->getVolumetricRep("VolumetricRep_"+index);
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

  connect(toolManager(), SIGNAL(initialized()), this, SLOT(toolsAvailableSlot()));
  toolsAvailableSlot();
}

void ViewWrapper3D::setImage(ssc::ImagePtr image)
{
   mImage = image;

   if (!mImage)
   {
     return;
   }
   if (!mImage->getRefVtkImageData().GetPointer())
   {
     messageManager()->sendWarning("ViewManager::setImage vtk image missing from current image!");
     return;
   }
    //Set these when image is deleted?
   //std::cout << "ViewGroup3D::setImage" << std::endl;
   mVolumetricRep->setImage(mImage);
   mProbeRep->setImage(mImage);
   mLandmarkRep->setImage(mImage);
   //std::cout << "ViewGroup3D::setImage" << std::endl;

   //update data name text rep
   mDataNameText->setText(0, mImage->getName());

   emit imageChanged(image->getUid().c_str());

   //Shading
   if(QSettings().value("shadingOn").toBool())
     mVolumetricRep->getVtkVolume()->GetProperty()->ShadeOn();

   mView->addRep(mVolumetricRep);
   mView->getRenderer()->ResetCamera();
   if(mView->isVisible())
     mView->getRenderWindow()->Render();
}

ssc::ImagePtr ViewWrapper3D::getImage() const
{
  return mImage;
}

ssc::View* ViewWrapper3D::getView()
{
  return mView;
}

void ViewWrapper3D::dominantToolChangedSlot()
{
  ssc::ToolPtr dominantTool = toolManager()->getDominantTool();
  mProbeRep->setTool(dominantTool);
  //std::cout << "ViewWrapper3D::dominantToolChangedSlot(): " << dominantTool.get() << std::endl;
}

void ViewWrapper3D::removeImage(ssc::ImagePtr image)
{
  if(mImage != image)
    return;

  messageManager()->sendInfo("remove image from view group 3d: "+image->getName());
  mView->removeRep(mVolumetricRep);

  mImage.reset();
}

void ViewWrapper3D::toolsAvailableSlot()
{
  std::cout <<"void ViewWrapper3D::toolsAvailableSlot() " << std::endl;
  // we want to do this also when nonconfigured and manual tool is present
//  if (!toolManager()->isConfigured())
//    return;

  ssc::ToolManager::ToolMapPtr tools = toolManager()->getConfiguredTools();
  ssc::ToolManager::ToolMapPtr::value_type::iterator iter;
  for (iter=tools->begin(); iter!=tools->end(); ++iter)
  {
    if(iter->second->getType() == ssc::Tool::TOOL_REFERENCE)
      continue;

    std::string uid = iter->second->getUid()+"_rep3d_"+this->mView->getUid();
    if (!mToolReps.count(uid))
    {
      mToolReps[uid] = ssc::ToolRep3D::New(uid);
      repManager()->addToolRep3D(mToolReps[uid]);
    }
    ssc::ToolRep3DPtr toolRep = mToolReps[uid];
    std::cout << "setting 3D tool rep for " << iter->second->getName() << std::endl;
    toolRep->setTool(iter->second);
    mView->addRep(toolRep);
  }

//
//  ToolRep3DMap* toolRep3DMap = repManager()->getToolRep3DReps();
//  ToolRep3DMap::iterator repIt = toolRep3DMap->begin();
//  ssc::ToolManager::ToolMapPtr tools = toolManager()->getTools();
//  ssc::ToolManager::ToolMap::iterator toolIt = tools->begin();
//
//  while((toolIt != tools->end()) && (repIt != toolRep3DMap->end()))
//  {
//    if(toolIt->second->getType() != ssc::Tool::TOOL_REFERENCE)
//    {
//      std::cout << "setting tool rep " << toolIt->second->getName() << std::endl;
//      repIt->second->setTool(toolIt->second);
//      mView->addRep(repIt->second);
//      repIt++;
//    }
//    toolIt++;
//  }
}


void ViewWrapper3D::setRegistrationMode(ssc::REGISTRATION_STATUS mode)
{
  if (mode==ssc::rsNOT_REGISTRATED)
  {
    mView->removeRep(mLandmarkRep);
    mView->removeRep(mProbeRep);
    
    disconnect(toolManager(), SIGNAL(dominantToolChanged(const std::string&)), this, SLOT(dominantToolChangedSlot()));
  }
  if (mode==ssc::rsIMAGE_REGISTRATED)
  {
    mView->addRep(mLandmarkRep);
    mView->addRep(mProbeRep);

    connect(toolManager(), SIGNAL(dominantToolChanged(const std::string&)), this, SLOT(dominantToolChangedSlot()));
    this->dominantToolChangedSlot();
  }
  if (mode==ssc::rsPATIENT_REGISTRATED)
  {
    mView->addRep(mLandmarkRep);
  }
}

//------------------------------------------------------------------------------


}
