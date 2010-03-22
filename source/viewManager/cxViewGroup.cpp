#include "cxViewGroup.h"

#include <QString>
#include <QPoint>
#include <QMenu>
#include <QSettings>
#include <QAction>
#include <vtkRenderWindow.h>
#include "sscView.h"
#include "sscSliceProxy.h"
#include "sscSlicerRepSW.h"
#include "sscTool2DRep.h"
#include "cxRepManager.h"
#include "cxDataManager.h"
#include "cxMessageManager.h"
#include "cxInriaRep2D.h"
#include "cxLandmarkRep.h"

namespace cx
{
ViewGroup::ViewGroup()
{
}
ViewGroup::~ViewGroup()
{}
std::string ViewGroup::toString(int i) const
{
  return QString::number(i).toStdString();
}
void ViewGroup::connectContextMenu()
{
  for(unsigned int i=0;i<mViews.size();++i)
  {
   connect(mViews[i], SIGNAL(customContextMenuRequested(const QPoint &)),
       this, SLOT(contexMenuSlot(const QPoint &)));
  }
}
void ViewGroup::contexMenuSlot(const QPoint& point)
{
  //NOT SUPPORTING MESHES IN 3D VIEW YET

  QWidget* sender = dynamic_cast<QWidget*>(this->sender());
  QPoint pointGlobal = sender->mapToGlobal(point);
  /*ssc::View* senderView = dynamic_cast<ssc::View*>(sender);
  if(senderView)
    std::cout << senderView->getUid() << std::endl;*/
  QMenu contextMenu(sender);

    //Get a list of available image and meshes names
    std::map<std::string, std::string> imageUidsAndNames = dataManager()->getImageUidsAndNames();
    std::map<std::string, std::string> meshUidsAndNames = dataManager()->getMeshUidsWithNames();

    //Display the lists to the user
    std::map<std::string, std::string>::iterator imageIt = imageUidsAndNames.begin();
    while(imageIt != imageUidsAndNames.end())
    {
      const QString uid = imageIt->first.c_str();
      const QString name = imageIt->second.c_str();
      QAction* imageAction = new QAction(name, &contextMenu);
      imageAction->setStatusTip(uid.toStdString().c_str());
      contextMenu.addAction(imageAction);
      imageIt++;
    }

    //Find out which the user chose
    QAction* theAction = contextMenu.exec(pointGlobal);
    if(!theAction)//this happens if you rightclick in the view and then don't select a action
      return;

    QString imageName = theAction->text();
    QString imageUid = theAction->statusTip();
    ssc::ImagePtr image = dataManager()->getImage(imageUid.toStdString());

    if(!image)
    {
      std::string error = "Couldn't find image with uid "+imageUid.toStdString()+" to set in View3D.";
      messageManager()->sendError(error);
      return;
    }

    this->setImage(image);
}
ViewGroupInria::ViewGroupInria(int startIndex, ssc::View* view1,
    ssc::View* view2, ssc::View* view3) :
    mStartIndex(startIndex)
{
  mViews.push_back(view1);
  mViews.push_back(view2);
  mViews.push_back(view3);

  this->connectContextMenu();
}
ViewGroupInria::~ViewGroupInria()
{}
void ViewGroupInria::setImage(ssc::ImagePtr image)
{
  mImage = image;
  RepManager* repManager = RepManager::getInstance();

  int baseIndex = 3*mStartIndex -2;
  InriaRep2DPtr inriaRep2D_1 = repManager->getInria2DRep("InriaRep2D_"+toString(baseIndex+0));
  InriaRep2DPtr inriaRep2D_2 = repManager->getInria2DRep("InriaRep2D_"+toString(baseIndex+1));
  InriaRep2DPtr inriaRep2D_3 = repManager->getInria2DRep("InriaRep2D_"+toString(baseIndex+2));

  inriaRep2D_1->setImage(mImage);
  inriaRep2D_2->setImage(mImage);
  inriaRep2D_3->setImage(mImage);

//  ssc::ProbeRepPtr probeRep = repManager->getProbeRep("ProbeRep_"+toString(mStartIndex));

  if (!mImage)
  {
    return;
  }
  if (!mImage->getRefVtkImageData().GetPointer())
  {
    messageManager()->sendWarning("ViewManager::currentImageChangedSlot vtk image missing from current image!");
    return;
  }

  mViews[0]->setRep(inriaRep2D_1);
  mViews[1]->setRep(inriaRep2D_2);
  mViews[2]->setRep(inriaRep2D_3);

  inriaRep2D_1->getVtkViewImage2D()->SetOrientation(vtkViewImage2D::AXIAL_ID);
  inriaRep2D_2->getVtkViewImage2D()->SetOrientation(vtkViewImage2D::CORONAL_ID);
  inriaRep2D_3->getVtkViewImage2D()->SetOrientation(vtkViewImage2D::SAGITTAL_ID);
  inriaRep2D_1->getVtkViewImage2D()->AddChild(inriaRep2D_2->getVtkViewImage2D());
  inriaRep2D_2->getVtkViewImage2D()->AddChild(inriaRep2D_3->getVtkViewImage2D());
  inriaRep2D_3->getVtkViewImage2D()->AddChild(inriaRep2D_1->getVtkViewImage2D());
  inriaRep2D_1->getVtkViewImage2D()->SyncRemoveAllDataSet();
  inriaRep2D_1->getVtkViewImage2D()->SyncAddDataSet(mImage->getRefVtkImageData());
  inriaRep2D_1->getVtkViewImage2D()->SyncReset();
/*
  //connecting proberep and inriareps
  //this happens every time an image changed
  //but its always the same proberep and inriareps we connect
  //this should really only be done once
  //TODO: maybe move this somewhere it«ll only be done once?
  connect(probeRep.get(), SIGNAL(pointPicked(double,double,double)),
          inriaRep2D_1.get(), SLOT(syncSetPosition(double,double,double)));
  connect(inriaRep2D_1.get(), SIGNAL(pointPicked(double,double,double)),
          probeRep.get(), SLOT(showTemporaryPointSlot(double,double,double)));
  connect(inriaRep2D_2.get(), SIGNAL(pointPicked(double,double,double)),
          probeRep.get(), SLOT(showTemporaryPointSlot(double,double,double)));
  connect(inriaRep2D_3.get(), SIGNAL(pointPicked(double,double,double)),
          probeRep.get(), SLOT(showTemporaryPointSlot(double,double,double)));
*/
}
void ViewGroupInria::removeImage(ssc::ImagePtr image)
{
  if(mImage!=image)
    return;

  messageManager()->sendInfo("removeImage from inria view group: "+image->getName());
  RepManager* repManager = RepManager::getInstance();

  int baseIndex = 3*mStartIndex -2;
  InriaRep2DPtr inriaRep2D_1 = repManager->getInria2DRep("InriaRep2D_"+toString(baseIndex+0));
  InriaRep2DPtr inriaRep2D_2 = repManager->getInria2DRep("InriaRep2D_"+toString(baseIndex+1));
  InriaRep2DPtr inriaRep2D_3 = repManager->getInria2DRep("InriaRep2D_"+toString(baseIndex+2));

/*  ssc::ProbeRepPtr probeRep = repManager->getProbeRep("ProbeRep_"+toString(mStartIndex));

  disconnect(probeRep.get(), SIGNAL(pointPicked(double,double,double)),
          inriaRep2D_1.get(), SLOT(syncSetPosition(double,double,double)));
  disconnect(inriaRep2D_1.get(), SIGNAL(pointPicked(double,double,double)),
          probeRep.get(), SLOT(showTemporaryPointSlot(double,double,double)));
  disconnect(inriaRep2D_2.get(), SIGNAL(pointPicked(double,double,double)),
          probeRep.get(), SLOT(showTemporaryPointSlot(double,double,double)));
  disconnect(inriaRep2D_3.get(), SIGNAL(pointPicked(double,double,double)),
          probeRep.get(), SLOT(showTemporaryPointSlot(double,double,double)));
*/


  if(inriaRep2D_1->hasImage(mImage))
  {
    mViews[0]->removeRep(inriaRep2D_1);
    mViews[1]->removeRep(inriaRep2D_2);
    mViews[2]->removeRep(inriaRep2D_3);
    inriaRep2D_1->getVtkViewImage2D()->SyncRemoveDataSet(image->getRefVtkImageData());

    // Test: create small dummy data set with one voxel
    vtkImageDataPtr dummyImageData = vtkImageData::New();
    dummyImageData->SetExtent(0,0,0,0,0,0);
    dummyImageData->SetSpacing(1,1,1);
    //dummyImageData->SetScalarTypeToUnsignedShort();
    dummyImageData->SetScalarTypeToUnsignedChar();
    dummyImageData->SetNumberOfScalarComponents(1);
    dummyImageData->AllocateScalars();
    unsigned char* dataPtr = static_cast<unsigned char*>(dummyImageData->GetScalarPointer());
    dataPtr = 0;//Set voxel to black
    inriaRep2D_1->getVtkViewImage2D()->SyncAddDataSet(dummyImageData);
    inriaRep2D_1->getVtkViewImage2D()->SyncReset();

    //emit imageDeletedFromViews(mImage);
    messageManager()->sendInfo("Removed current image from inria view group");
  }
  mImage.reset();
}

void ViewGroupInria::setRegistrationMode(ssc::REGISTRATION_STATUS mode)
{
  RepManager* repManager = RepManager::getInstance();
  int baseIndex = 3*mStartIndex -2;
  InriaRep2DPtr inriaRep2D_1 = repManager->getInria2DRep("InriaRep2D_"+toString(baseIndex+0));
  InriaRep2DPtr inriaRep2D_2 = repManager->getInria2DRep("InriaRep2D_"+toString(baseIndex+1));
  InriaRep2DPtr inriaRep2D_3 = repManager->getInria2DRep("InriaRep2D_"+toString(baseIndex+2));

  ssc::ProbeRepPtr probeRep = repManager->getProbeRep("ProbeRep_"+toString(mStartIndex));

  if (mode==ssc::rsIMAGE_REGISTRATED)
  {
    connect(probeRep.get(),     SIGNAL(pointPicked(double,double,double)),inriaRep2D_1.get(), SLOT(syncSetPosition(double,double,double)));
    connect(inriaRep2D_1.get(), SIGNAL(pointPicked(double,double,double)),probeRep.get(), SLOT(showTemporaryPointSlot(double,double,double)));
    connect(inriaRep2D_2.get(), SIGNAL(pointPicked(double,double,double)),probeRep.get(), SLOT(showTemporaryPointSlot(double,double,double)));
    connect(inriaRep2D_3.get(), SIGNAL(pointPicked(double,double,double)),probeRep.get(), SLOT(showTemporaryPointSlot(double,double,double)));
  }
  if (mode!=ssc::rsIMAGE_REGISTRATED)
  {
    disconnect(probeRep.get(),     SIGNAL(pointPicked(double,double,double)),inriaRep2D_1.get(), SLOT(syncSetPosition(double,double,double)));
    disconnect(inriaRep2D_1.get(), SIGNAL(pointPicked(double,double,double)),probeRep.get(), SLOT(showTemporaryPointSlot(double,double,double)));
    disconnect(inriaRep2D_2.get(), SIGNAL(pointPicked(double,double,double)),probeRep.get(), SLOT(showTemporaryPointSlot(double,double,double)));
    disconnect(inriaRep2D_3.get(), SIGNAL(pointPicked(double,double,double)),probeRep.get(), SLOT(showTemporaryPointSlot(double,double,double)));
  }
}


//------------------------------------------------------------------------------

ViewGroup3D::ViewGroup3D(int startIndex, ssc::View* view) :
    mStartIndex(startIndex)
{
  RepManager* repManager = RepManager::getInstance();
  mViews.push_back(view);
  mVolumetricRep = repManager->getVolumetricRep("VolumetricRep_"+toString(mStartIndex));
  mLandmarkRep = repManager->getLandmarkRep("LandmarkRep_"+toString(mStartIndex));
  mProbeRep = repManager->getProbeRep("ProbeRep_"+toString(mStartIndex));

  this->connectContextMenu();
}
ViewGroup3D::~ViewGroup3D()
{}
void ViewGroup3D::setImage(ssc::ImagePtr image)
{
  std::cout << "ViewGroup3D::setImage B" << std::endl;
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

  mViews[0]->addRep(mVolumetricRep);
  mViews[0]->getRenderer()->ResetCamera();
  if(mViews[0]->isVisible())
    mViews[0]->getRenderWindow()->Render();
  std::cout << "ViewGroup3D::setImage E" << std::endl;
}
void ViewGroup3D::removeImage(ssc::ImagePtr image)
{
  if(mImage != image)
    return;

  messageManager()->sendInfo("remove image from view group 3d: "+image->getName());
  mViews[0]->removeRep(mVolumetricRep);

  mImage.reset();
}

void ViewGroup3D::setRegistrationMode(ssc::REGISTRATION_STATUS mode)
{
  if (mode==ssc::rsNOT_REGISTRATED)
  {
    mViews[0]->removeRep(mLandmarkRep);
    mViews[0]->removeRep(mProbeRep);
  }
  if (mode==ssc::rsIMAGE_REGISTRATED)
  {
    mViews[0]->addRep(mLandmarkRep);
    mViews[0]->addRep(mProbeRep);
  }
  if (mode==ssc::rsPATIENT_REGISTRATED)
  {
    mViews[0]->addRep(mLandmarkRep);
  }
}

//------------------------------------------------------------------------------

ViewGroup2D::ViewGroup2D(int startIndex, ssc::View* view1,
    ssc::View* view2, ssc::View* view3)
{
  mStartIndex = startIndex;

  mViews.push_back(view1);
  mViews.push_back(view2);
  mViews.push_back(view3);

  this->connectContextMenu();

  mElements.resize(3);


  for (unsigned i=0; i<mElements.size(); ++i)
  {
    mElements[i].mSliceProxy.reset(new ssc::SliceProxy());
    //mSliceProxy->setTool(tool);
  }

  mElements[0].mSliceProxy->initializeFromPlane(ssc::ptAXIAL, false, ssc::Vector3D(0,0,1), false, 1, 0.25);
  mElements[1].mSliceProxy->initializeFromPlane(ssc::ptCORONAL, false, ssc::Vector3D(0,0,1), false, 1, 0.25);
  mElements[2].mSliceProxy->initializeFromPlane(ssc::ptSAGITTAL, false, ssc::Vector3D(0,0,1), false, 1, 0.25);

  for (unsigned i=0; i<mElements.size(); ++i)
  {
    //mElements[i].mSliceProxy->setTool(...);
    mElements[i].mSliceRep = ssc::SliceRepSW::New("SliceRep_"+mViews[i]->getName());
 //   mElements[i].mToolRep2D.reset(new ssc::ToolRep2D());
 //   mElements[i].mSliceRep->setProxy(mElements[i].mSliceProxy);
 //   mElements[i].mToolRep2D->setProxy(mElements[i].mSliceProxy);
  }
}

ViewGroup2D::~ViewGroup2D()
{

}

void ViewGroup2D::setImage(ssc::ImagePtr image)
{

}

void ViewGroup2D::removeImage(ssc::ImagePtr image)
{

}

}//cx
