/*
 * cxViewWrapper3D.cpp
 *
 *  Created on: Mar 24, 2010
 *      Author: christiana
 */

#include "cxViewWrapper3D.h"
#include <vector>
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

  connect(toolManager(), SIGNAL(configured()), this, SLOT(toolManagerConfiguredSlot()));
  toolManagerConfiguredSlot();
}

void ViewWrapper3D::setImage(ssc::ImagePtr image)
{
   mImage = image;
   //RepManager* repManager = RepManager::getInstance();

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

   //Shading
   if(QSettings().value("shadingOn").toBool())
     mVolumetricRep->getVtkVolume()->GetProperty()->ShadeOn();

   mView->addRep(mVolumetricRep);
   mView->getRenderer()->ResetCamera();
   if(mView->isVisible())
     mView->getRenderWindow()->Render();
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

void ViewWrapper3D::toolManagerConfiguredSlot()
{
  if (!toolManager()->isConfigured())
    return;

  ToolRep3DMap* toolRep3DMap = repManager()->getToolRep3DReps();
  ToolRep3DMap::iterator repIt = toolRep3DMap->begin();
  ssc::ToolManager::ToolMapPtr configuredTools = toolManager()->getConfiguredTools();
  ssc::ToolManager::ToolMap::iterator toolIt = configuredTools->begin();

  while((toolIt != configuredTools->end()) && (repIt != toolRep3DMap->end()))
  {
    if(toolIt->second->getType() != ssc::Tool::TOOL_REFERENCE)
    {
      repIt->second->setTool(toolIt->second);
      mView->addRep(repIt->second);
      repIt++;
    }
    toolIt++;
  }
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
