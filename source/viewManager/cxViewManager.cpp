#include "cxViewManager.h"

#include <QGridLayout>
#include <QWidget>
#include <QTimer>
#include <QSettings>
#include <QTime>
#include <vtkRenderWindow.h>
#include <vtkImageData.h>
#include "sscProbeRep.h"
#include "sscVolumetricRep.h"
#include "cxMessageManager.h"
#include "cxRepManager.h"
#include "cxView2D.h"
#include "cxView3D.h"

namespace cx
{

ViewManager *ViewManager::mTheInstance = NULL;

ViewManager* ViewManager::getInstance()
{
  if(mTheInstance == NULL)
   {
     mTheInstance = new ViewManager();
   }
   return mTheInstance;
}
void ViewManager::destroyInstance()
{}
ViewManager::ViewManager() :
  mCurrentLayoutType(LAYOUT_NONE),
  mLayout(new QGridLayout()),
  mMainWindowsCentralWidget(new QWidget()),
  MAX_3DVIEWS(2),
  MAX_2DVIEWS(9),
  mRenderingTimer(new QTimer(this)),
  mSettings(new QSettings()),
  mRenderingTime(new QTime()),
  mNumberOfRenderings(0)
{
  mLayout->setSpacing(1);
  mMainWindowsCentralWidget->setLayout(mLayout);

  mView3DNames[0] = "View3D_1";
  mView3DNames[1] = "View3D_2";
  mView2DNames[0] = "View2D_1";
  mView2DNames[1] = "View2D_2";
  mView2DNames[2] = "View2D_3";
  mView2DNames[3] = "View2D_4";
  mView2DNames[4] = "View2D_5";
  mView2DNames[5] = "View2D_6";
  mView2DNames[6] = "View2D_7";
  mView2DNames[7] = "View2D_8";
  mView2DNames[8] = "View2D_9";

  for(int i=0; i<MAX_3DVIEWS; i++)
  {
    View3D* view = new View3D(mView3DNames[i], mView3DNames[i],
                              mMainWindowsCentralWidget);
    view->hide();
    mView3DMap[view->getUid()] = view;
    
    //Turn off rendering in vtkRenderWindowInteractor
    view->getRenderWindow()->GetInteractor()->EnableRenderOff();
  }
  for(int i=0; i<MAX_2DVIEWS; i++)
  {
    View2D* view = new View2D(mView2DNames[i], mView2DNames[i],
                              mMainWindowsCentralWidget);
    view->hide();
    mView2DMap[view->getUid()] = view;
    
    //Turn off rendering in vtkRenderWindowInteractor
    view->getRenderWindow()->GetInteractor()->EnableRenderOff();
  }

  this->setLayoutTo_3DACS_2X2();

  mRenderingTimer->start(mSettings->value("renderingInterval").toInt());
  connect(mRenderingTimer, SIGNAL(timeout()),
          this, SLOT(renderAllViewsSlot()));
  
  mShadingOn = mSettings->value("shadingOn").toBool();
  
  mRenderingTime->start();
}
ViewManager::~ViewManager()
{}
QWidget* ViewManager::stealCentralWidget()
{
  return mMainWindowsCentralWidget;
}
ViewManager::View2DMap* ViewManager::get2DViews()
{
  return &mView2DMap;
}
ViewManager::View3DMap* ViewManager::get3DViews()
{
  return &mView3DMap;
}
ViewManager::LayoutType ViewManager::getCurrentLayoutType()
{
  return mCurrentLayoutType;
}
ssc::View* ViewManager::getView(const std::string& uid)
{
  ssc::View* view = NULL;
  View2DMap::iterator it2 = mView2DMap.find(uid);
  if(it2 != mView2DMap.end())
  {
    view = (*it2).second;
  }
  View3DMap::iterator it3 = mView3DMap.find(uid);
  if(it3 != mView3DMap.end())
  {
    view = (*it3).second;
  }
  return view;
}
View2D* ViewManager::get2DView(const std::string& uid)
{
  View2D* view = NULL;
  View2DMap::iterator it2 = mView2DMap.find(uid);
  if(it2 != mView2DMap.end())
  {
    view = (*it2).second;
  }
  return view;
}
View3D* ViewManager::get3DView(const std::string& uid)
{
  View3D* view = NULL;
  View3DMap::iterator it3 = mView3DMap.find(uid);
  if(it3 != mView3DMap.end())
  {
    view = (*it3).second;
  }
  return view;
}
void ViewManager::setLayoutTo_3D_1X1()
{
  switch(mCurrentLayoutType)
  {
  case LAYOUT_NONE:
    this->activateLayout_3D_1X1();
    break;
  case LAYOUT_3D_1X1:
    break;
  case LAYOUT_3DACS_2X2:
    this->deactivateLayout_3DACS_2X2();
    this->activateLayout_3D_1X1();
    break;
  case LAYOUT_3DACS_1X3:
    this->deactivateLayout_3DACS_1X3();
    this->activateLayout_3D_1X1();
    break;
  case LAYOUT_ACSACS_2X3:
    this->deactivateLayout_ACSACS_2X3();
    this->activateLayout_3D_1X1();
    break;
  default:
    return;
    break;
  }
  messageMan()->sendInfo("Layout changed to 3D_1X1");
}
void ViewManager::setLayoutTo_3DACS_2X2()
{
  switch(mCurrentLayoutType)
  {
  case LAYOUT_NONE:
    this->activateLayout_3DACS_2X2();
    break;
  case LAYOUT_3D_1X1:
    this->deactivatLayout_3D_1X1();
    this->activateLayout_3DACS_2X2();
    break;
  case LAYOUT_3DACS_2X2:
    break;
  case LAYOUT_3DACS_1X3:
    this->deactivateLayout_3DACS_1X3();
    this->activateLayout_3DACS_2X2();
    break;
  case LAYOUT_ACSACS_2X3:
    this->deactivateLayout_ACSACS_2X3();
    this->activateLayout_3DACS_2X2();
    break;
  default:
    return;
    break;
  }
  messageMan()->sendInfo("Layout changed to 3DACS_2X2");
}
void ViewManager::setLayoutTo_3DACS_1X3()
{
  switch(mCurrentLayoutType)
  {
  case LAYOUT_NONE:
    this->activateLayout_3DACS_1X3();
    break;
  case LAYOUT_3D_1X1:
    this->deactivatLayout_3D_1X1();
    this->activateLayout_3DACS_1X3();
    break;
  case LAYOUT_3DACS_2X2:
    this->deactivateLayout_3DACS_2X2();
    this->activateLayout_3DACS_1X3();
    break;
  case LAYOUT_3DACS_1X3:
    break;
  case LAYOUT_ACSACS_2X3:
    this->deactivateLayout_ACSACS_2X3();
    this->activateLayout_3DACS_1X3();
    break;
  default:
    return;
    break;
  }
  messageMan()->sendInfo("Layout changed to 3DACS_1X3");
}
void ViewManager::setLayoutTo_ACSACS_2X3()
{
  switch(mCurrentLayoutType)
  {
  case LAYOUT_NONE:
    this->activateLayout_ACSACS_2X3();
    break;
  case LAYOUT_3D_1X1:
    this->deactivatLayout_3D_1X1();
    this->activateLayout_ACSACS_2X3();
    break;
  case LAYOUT_3DACS_2X2:
    this->deactivateLayout_3DACS_2X2();
    this->activateLayout_ACSACS_2X3();
    break;
  case LAYOUT_3DACS_1X3:
    this->deactivateLayout_3DACS_1X3();
    this->activateLayout_ACSACS_2X3();
    break;
  case LAYOUT_ACSACS_2X3:
    break;
  default:
    return;
    break;
  }
  messageMan()->sendInfo("Layout changed to ACSACS_2X3");
}
  
void ViewManager::deleteImageSlot(ssc::ImagePtr image)
{
  messageMan()->sendInfo("Delete image: "+image->getName());
  RepManager* repManager = RepManager::getInstance();
  VolumetricRepMap* volRepMap = repManager->getVolumetricReps();
  VolumetricRepMap::iterator itVolRep = volRepMap->begin();
  for(; itVolRep != volRepMap->end(); ++itVolRep)
    if(itVolRep->second->hasImage(image))
      this->removeRepFromViews(itVolRep->second);
  
  /*InriaRep3DMap* inria3DRepMap = repManager->getInria3DReps();
  InriaRep3DMap::iterator itInria3DRep = inria3DRepMap->begin();
  for(; itInria3DRep != inria3DRepMap->end(); ++itInria3DRep)
    if(itInria3DRep->second->hasImage(image))
      this->removeRepFromViews(itInria3DRep->second);*/
  
  /*InriaRep2DMap* inria2DRepMap = repManager->getInria2DReps();
  InriaRep2DMap::iterator itInria2DRep = inria2DRepMap->begin();
  for(; itInria2DRep != inria2DRepMap->end(); ++itInria2DRep)
    if(itInria2DRep->second->hasImage(image))
      this->removeRepFromViews(itInria2DRep->second);*/
  
  InriaRep2DPtr inriaRep2D_1 = repManager->getInria2DRep("InriaRep2D_1");
  InriaRep2DPtr inriaRep2D_2 = repManager->getInria2DRep("InriaRep2D_2");
  InriaRep2DPtr inriaRep2D_3 = repManager->getInria2DRep("InriaRep2D_3");
  
  //Don't work?
  //if(inriaRep2D_1->getVtkViewImage2D()->HasDataSet(image->getRefVtkImageData()))
  if(inriaRep2D_1->hasImage(image))
  {  
    View2D* view2D_1 = mView2DMap[mView2DNames[0]];
    View2D* view2D_2 = mView2DMap[mView2DNames[1]];
    View2D* view2D_3 = mView2DMap[mView2DNames[2]];
    view2D_1->removeRep(inriaRep2D_1);
    view2D_2->removeRep(inriaRep2D_2);
    view2D_3->removeRep(inriaRep2D_3);
    //Don't work?
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
    
    //this->renderAllViewsSlot();
    //inriaRep2D_1->getVtkViewImage2D()->SyncRemoveAllDataSet();
    emit imageDeletedFromViews(image);
    messageMan()->sendInfo("Removed current image from inria views");
  }
}

void ViewManager::renderingIntervalChangedSlot(int interval)
{
  mRenderingTimer->stop();
  mRenderingTimer->start(interval);
}

void ViewManager::shadingChangedSlot(bool shadingOn)
{
  mShadingOn = shadingOn;
  
  ssc::VolumetricRepPtr volumetricRep
  = RepManager::getInstance()->getVolumetricRep("VolumetricRep_1");
  if(volumetricRep->getImage())
    if(shadingOn)
      volumetricRep->getVtkVolume()->GetProperty()->ShadeOn();
    else
      volumetricRep->getVtkVolume()->GetProperty()->ShadeOff();
}
  
void ViewManager::activateLayout_3D_1X1()
{
  mLayout->addWidget(mView3DMap[mView3DNames[0]]);
  mView3DMap[mView3DNames[0]]->show();
  mCurrentLayoutType = LAYOUT_3D_1X1;
}
void ViewManager::deactivatLayout_3D_1X1()
{
  mView3DMap[mView3DNames[0]]->hide();
  mLayout->removeWidget(mView3DMap[mView3DNames[0]]);
}
void ViewManager::activateLayout_3DACS_2X2()
{
  mLayout->addWidget( mView3DMap[mView3DNames[0]],   0, 0 );
  mLayout->addWidget( mView2DMap[mView2DNames[0]],   0, 1 );
  mLayout->addWidget( mView2DMap[mView2DNames[1]],   1, 0 );
  mLayout->addWidget( mView2DMap[mView2DNames[2]],   1, 1 );
  mView3DMap[mView3DNames[0]]->show();
  mView2DMap[mView2DNames[0]]->show();
  mView2DMap[mView2DNames[1]]->show();
  mView2DMap[mView2DNames[2]]->show();

  mCurrentLayoutType = LAYOUT_3DACS_2X2;
}
void ViewManager::deactivateLayout_3DACS_2X2()
{
  mView3DMap[mView3DNames[0]]->hide();
  mView2DMap[mView2DNames[0]]->hide();
  mView2DMap[mView2DNames[1]]->hide();
  mView2DMap[mView2DNames[2]]->hide();
  mLayout->removeWidget( mView3DMap[mView3DNames[0]]);
  mLayout->removeWidget( mView2DMap[mView2DNames[0]]);
  mLayout->removeWidget( mView2DMap[mView2DNames[1]]);
  mLayout->removeWidget( mView2DMap[mView2DNames[2]]);
}
void ViewManager::activateLayout_3DACS_1X3()
{
  mLayout->addWidget( mView3DMap[mView3DNames[0]],   0, 0, 3, 1 );
  mLayout->addWidget( mView2DMap[mView2DNames[0]],   0, 1 );
  mLayout->addWidget( mView2DMap[mView2DNames[1]],   1, 1 );
  mLayout->addWidget( mView2DMap[mView2DNames[2]],   2, 1 );
  mView3DMap[mView3DNames[0]]->show();
  mView2DMap[mView2DNames[0]]->show();
  mView2DMap[mView2DNames[1]]->show();
  mView2DMap[mView2DNames[2]]->show();
  mCurrentLayoutType = LAYOUT_3DACS_1X3;
}
void ViewManager::deactivateLayout_3DACS_1X3()
{
  mView3DMap[mView3DNames[0]]->hide();
  mView2DMap[mView2DNames[0]]->hide();
  mView2DMap[mView2DNames[1]]->hide();
  mView2DMap[mView2DNames[2]]->hide();
  mLayout->removeWidget( mView3DMap[mView3DNames[0]]);
  mLayout->removeWidget( mView2DMap[mView2DNames[0]]);
  mLayout->removeWidget( mView2DMap[mView2DNames[1]]);
  mLayout->removeWidget( mView2DMap[mView2DNames[2]]);
}
void ViewManager::activateLayout_ACSACS_2X3()
{
  mLayout->addWidget( mView2DMap[mView2DNames[0]],   0, 0);
  mLayout->addWidget( mView2DMap[mView2DNames[1]],   0, 1 );
  mLayout->addWidget( mView2DMap[mView2DNames[2]],   0, 2 );
  mLayout->addWidget( mView2DMap[mView2DNames[3]],   1, 0 );
  mLayout->addWidget( mView2DMap[mView2DNames[4]],   1, 1 );
  mLayout->addWidget( mView2DMap[mView2DNames[5]],   1, 2 );
  mView2DMap[mView2DNames[0]]->show();
  mView2DMap[mView2DNames[1]]->show();
  mView2DMap[mView2DNames[2]]->show();
  mView2DMap[mView2DNames[3]]->show();
  mView2DMap[mView2DNames[4]]->show();
  mView2DMap[mView2DNames[5]]->show();
  mCurrentLayoutType = LAYOUT_ACSACS_2X3;
}
void ViewManager::deactivateLayout_ACSACS_2X3()
{
  mView2DMap[mView2DNames[0]]->hide();
  mView2DMap[mView2DNames[1]]->hide();
  mView2DMap[mView2DNames[2]]->hide();
  mView2DMap[mView2DNames[3]]->hide();
  mView2DMap[mView2DNames[4]]->hide();
  mView2DMap[mView2DNames[5]]->hide();
  mLayout->removeWidget( mView2DMap[mView2DNames[0]]);
  mLayout->removeWidget( mView2DMap[mView2DNames[1]]);
  mLayout->removeWidget( mView2DMap[mView2DNames[2]]);
  mLayout->removeWidget( mView2DMap[mView2DNames[3]]);
  mLayout->removeWidget( mView2DMap[mView2DNames[4]]);
  mLayout->removeWidget( mView2DMap[mView2DNames[5]]);
}
  
void ViewManager::removeRepFromViews(ssc::RepPtr rep)
{
  View3DMap::iterator it3D = mView3DMap.begin();
  for(; it3D != mView3DMap.end(); ++it3D)
    it3D->second->removeRep(rep);
  View2DMap::iterator it2D = mView2DMap.begin();
  for(; it2D != mView2DMap.end(); ++it2D)
    it2D->second->removeRep(rep);
}
  
void ViewManager::renderAllViewsSlot()
{
  View3DMap::iterator it3D = mView3DMap.begin();
  for(; it3D != mView3DMap.end(); ++it3D)
  {
    if(it3D->second->isVisible())
      it3D->second->getRenderWindow()->Render();
  }
  View2DMap::iterator it2D = mView2DMap.begin();
  for(; it2D != mView2DMap.end(); ++it2D)
  {
    if(it2D->second->isVisible())
      it2D->second->getRenderWindow()->Render();
  }
  
  if(mRenderingTime->elapsed()>1000)
  {
    emit fps(mNumberOfRenderings);
    mRenderingTime->restart();
    mNumberOfRenderings = 1;
  }
  else
    mNumberOfRenderings++;
}
	
void ViewManager::currentImageChangedSlot(ssc::ImagePtr currentImage)
{  
  RepManager* repManager = RepManager::getInstance();
  
  // Update 2D views
  InriaRep2DPtr inriaRep2D_1 = repManager->getInria2DRep("InriaRep2D_1");
  InriaRep2DPtr inriaRep2D_2 = repManager->getInria2DRep("InriaRep2D_2");
  InriaRep2DPtr inriaRep2D_3 = repManager->getInria2DRep("InriaRep2D_3");
  inriaRep2D_1->setImage(currentImage);
  inriaRep2D_2->setImage(currentImage);
  inriaRep2D_3->setImage(currentImage);
  View2D* view2D_1 = mView2DMap[mView2DNames[0]];
  View2D* view2D_2 = mView2DMap[mView2DNames[1]];
  View2D* view2D_3 = mView2DMap[mView2DNames[2]];
  ssc::ProbeRepPtr probeRep = repManager->getProbeRep("ProbeRep_1");
  
  if (!currentImage)
  {
    return;
  }
  if (!currentImage->getRefVtkImageData().GetPointer())
  {
    messageMan()->sendWarning("ViewManager::currentImageChangedSlot vtk image missing from current image!");
    return;
  }
  
  // Update 3D view
  ssc::VolumetricRepPtr volumetricRep
  = repManager->getVolumetricRep("VolumetricRep_1");
  LandmarkRepPtr landmarkRep = repManager->getLandmarkRep("LandmarkRep_1");
  
   //Set these when image is deleted?
  volumetricRep->setImage(currentImage);
  probeRep->setImage(currentImage);
  landmarkRep->setImage(currentImage);
  
  //Shading
  if(mShadingOn)
    volumetricRep->getVtkVolume()->GetProperty()->ShadeOn();
  
  View3D* view3D_1 = mView3DMap[mView3DNames[0]];
  view3D_1->setRep(volumetricRep);
  view3D_1->getRenderer()->ResetCamera();
  if(view3D_1->isVisible())
    view3D_1->getRenderWindow()->Render();

  view2D_1->setRep(inriaRep2D_1);
  view2D_2->setRep(inriaRep2D_2);
  view2D_3->setRep(inriaRep2D_3);
  
  //test: Is render set?
  /*if (inriaRep2D_1->getVtkViewImage2D()->GetRenderer() == NULL)
    std::cout << "inriaRep2D_1: Lost renderer" << std::endl;
  if (inriaRep2D_2->getVtkViewImage2D()->GetRenderer() == NULL)
    std::cout << "inriaRep2D_2: Lost renderer" << std::endl;
  if (inriaRep2D_3->getVtkViewImage2D()->GetRenderer() == NULL)
    std::cout << "inriaRep2D_3: Lost renderer" << std::endl;*/
  
  inriaRep2D_1->getVtkViewImage2D()->SetOrientation(vtkViewImage2D::AXIAL_ID);
  inriaRep2D_2->getVtkViewImage2D()->SetOrientation(vtkViewImage2D::CORONAL_ID);
  inriaRep2D_3->getVtkViewImage2D()->SetOrientation(vtkViewImage2D::SAGITTAL_ID);
  inriaRep2D_1->getVtkViewImage2D()->AddChild(inriaRep2D_2->getVtkViewImage2D());
  inriaRep2D_2->getVtkViewImage2D()->AddChild(inriaRep2D_3->getVtkViewImage2D());
  inriaRep2D_3->getVtkViewImage2D()->AddChild(inriaRep2D_1->getVtkViewImage2D());
  inriaRep2D_1->getVtkViewImage2D()->SyncRemoveAllDataSet();
  //TODO: ...or getBaseVtkImageData()???
  inriaRep2D_1->getVtkViewImage2D()->SyncAddDataSet(currentImage->getRefVtkImageData());
  inriaRep2D_1->getVtkViewImage2D()->SyncReset();
  
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
}	
}//namespace cx
