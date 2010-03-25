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
#include "cxMessageManager.h"
#include "cxInriaRep2D.h"
#include "cxLandmarkRep.h"

namespace cx
{

ViewWrapper3D::ViewWrapper3D(int startIndex, ssc::View* view)
{
  mView = view;
  RepManager* repManager = RepManager::getInstance();
  std::string index = QString::number(startIndex).toStdString();

  mVolumetricRep = repManager->getVolumetricRep("VolumetricRep_"+index);
  mLandmarkRep = repManager->getLandmarkRep("LandmarkRep_"+index);
  mProbeRep = repManager->getProbeRep("ProbeRep_"+index);
}

void ViewWrapper3D::setImage(ssc::ImagePtr image)
{
  // std::cout << "ViewGroup3D::setImage B" << std::endl;
   mImage = image;
   //RepManager* repManager = RepManager::getInstance();

   if (!mImage)
   {
     return;
   }
   if (!mImage->getRefVtkImageData().GetPointer())
   {
     messageManager()->sendWarning("ViewManager::currentImageChangedSlot vtk image missing from current image!");
     return;
   }
    //Set these when image is deleted?
   std::cout << "ViewGroup3D::setImage" << std::endl;
   mVolumetricRep->setImage(mImage);
   mProbeRep->setImage(mImage);
   mLandmarkRep->setImage(mImage);
   std::cout << "ViewGroup3D::setImage" << std::endl;

   //Shading
   if(QSettings().value("shadingOn").toBool())
     mVolumetricRep->getVtkVolume()->GetProperty()->ShadeOn();

   mView->addRep(mVolumetricRep);
   mView->getRenderer()->ResetCamera();
   if(mView->isVisible())
     mView->getRenderWindow()->Render();
   //std::cout << "ViewGroup3D::setImage E" << std::endl;
}

ssc::View* ViewWrapper3D::getView()
{
  return mView;
}

void ViewWrapper3D::removeImage(ssc::ImagePtr image)
{
  if(mImage != image)
    return;

  messageManager()->sendInfo("remove image from view group 3d: "+image->getName());
  mView->removeRep(mVolumetricRep);

  mImage.reset();
}

void ViewWrapper3D::setRegistrationMode(ssc::REGISTRATION_STATUS mode)
{
  if (mode==ssc::rsNOT_REGISTRATED)
  {
    mView->removeRep(mLandmarkRep);
    mView->removeRep(mProbeRep);
  }
  if (mode==ssc::rsIMAGE_REGISTRATED)
  {
    mView->addRep(mLandmarkRep);
    mView->addRep(mProbeRep);
  }
  if (mode==ssc::rsPATIENT_REGISTRATED)
  {
    mView->addRep(mLandmarkRep);
  }
}

//------------------------------------------------------------------------------


}
